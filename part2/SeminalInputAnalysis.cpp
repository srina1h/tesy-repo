#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <sstream>
#include <fstream>

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace
{
    // List of user input functions to be monitored
    std::vector<std::string> userInputFunctions = {
        "fopen",
        "freopen",
        "fscanf",
        "scanf",
        "sscanf",
        "fgetc",
        "fgets",
        "getc",
        "getchar",
        "gets",
        "get_s",
        "fread",
        "fgetwc",
        "getwc",
    };

    class SeminalInputAnalysis : public PassInfoMixin<SeminalInputAnalysis>
    {
    public:
        /**
         * @brief Serves as the entry point for the analysis (main function of the pass)
         * @param M The module to be analyzed
         * @param MAM The module analysis manager
         * @return PreservedAnalyses
         */
        PreservedAnalyses run(Module &M, ModuleAnalysisManager &MAM)
        {
            writeToFile("Seminal Input Analysis\n");

            // OBJECTIVE: Find conditional branching

            for (Function &F : M)
            {
                for (BasicBlock &BB : F)
                {
                    for (Instruction &I : BB)
                    {
                        if (auto *branchInst = dyn_cast<BranchInst>(&I))
                        {
                            if (branchInst->isConditional())
                            {
                                branches.push_back(branchInst);
                            }
                        }

                        if (auto *asDbgInst = dyn_cast<DbgDeclareInst>(&I))
                        {
                            processDbgInst(asDbgInst);
                        }
                    }
                }
            }

            // OBJECTIVE: Go through the call instructions and find the ways in which our mapped variables are used

            for (Function &F : M)
            {
                for (BasicBlock &BB : F)
                {
                    for (Instruction &I : BB)
                    {
                        if (auto asCallInst = dyn_cast<CallInst>(&I))
                        {
                            if (asCallInst->getCalledFunction())
                            {
                                processCallInstruction(asCallInst);
                            }
                        }
                    }
                }
            }

            analyzeAndPrintOutput();

            return PreservedAnalyses::all();
        }

    private:
        /*
        Define the variables that will be used throughout the analysis.
        */

        // Flag to check if file is created or not
        bool flag_for_file = false;

        std::map<Value *, std::string> variables;
        // Maps LLVM Values to their corresponding variable names as found in the source code.
        // This is used to associate LLVM's internal representation with human-readable variable names.

        std::map<Value *, std::set<Instruction *>> dependentInstructions;
        // Maps a call instruction to a set of instructions that are dependent on it.
        // This helps in tracking the flow and influence of function calls throughout the program.

        std::vector<BranchInst *> branches;
        // A list of all conditional branch instructions found during the analysis.
        // These are the branches that will be analyzed for user input dependencies.

        std::map<Value *, std::string> calledFunc;
        // Maps LLVM Values, specifically function call instructions, to their corresponding function names.
        // This aids in identifying which function calls are present in the analyzed code.

        void processDbgInst(DbgDeclareInst *asDbgInst)
        {
            // Extract the variable name from the debug information.
            std::string variableName = asDbgInst->getVariable()->getName().str();

            // Map the address of the variable to its name.
            variables[asDbgInst->getAddress()] = variableName;

            // Collect all instructions that use this variable.
            std::set<Instruction *> varUsers;
            defUseAnalysis(asDbgInst->getAddress(), varUsers);

            // Iterate over all users of the variable.
            for (auto user : varUsers)
            {
                // If the user is a Load instruction, map the instruction to the variable name.
                if (auto loadInst = dyn_cast<LoadInst>(user))
                {
                    if (loadInst->getOperand(0) == asDbgInst->getAddress())
                    {
                        variables[loadInst] = variableName;
                    }
                }
                // Similarly, for Store instructions, map the instruction to the variable name.
                else if (auto storeInst = dyn_cast<StoreInst>(user))
                {
                    if (storeInst->getOperand(1) == asDbgInst->getAddress())
                    {
                        variables[storeInst] = variableName;
                    }
                }
                // For Unary instructions, directly map the instruction to the variable name.
                else if (auto unaryInst = dyn_cast<UnaryInstruction>(user))
                {
                    variables[unaryInst] = variableName;
                }
            }
        }

        void defUseAnalysis(Value *value, std::set<Instruction *> &dependents)
        {
            // Check if the value is null to prevent processing an invalid value.
            if (!value)
            {
                return;
            }

            // Iterate over all users of the value.
            for (User *user : value->users())
            {
                // Cast the user to an Instruction, if possible.
                if (Instruction *inst = dyn_cast<Instruction>(user))
                {
                    // Avoid re-processing an instruction already in the set.
                    if (dependents.find(inst) != dependents.end())
                    {
                        return;
                    }

                    // Add the instruction to the set of users.
                    dependents.insert(inst);

                    // Special handling for Store instructions: recursively collect users of the pointer operand.
                    if (isa<StoreInst>(inst))
                    {
                        auto storeInst = dyn_cast<StoreInst>(inst);
                        defUseAnalysis(storeInst->getPointerOperand(), dependents);
                    }
                    // Special handling for Return instructions: if it returns a value, recursively collect users.
                    else if (isa<ReturnInst>(inst))
                    {
                        auto asRetInstruction = dyn_cast<ReturnInst>(inst);
                        if (asRetInstruction->getReturnValue())
                        {
                            defUseAnalysis(asRetInstruction->getFunction(), dependents);
                        }
                    }

                    // Recursively collect users of this instruction.
                    defUseAnalysis(inst, dependents);
                }
            }
        }

        void processCallInstruction(CallInst *callInstruction)
        {
            // Extract the name of the function being called.
            std::string calledFunctionName = callInstruction->getCalledFunction()->getName().str();
            // Clean any compiler-specific naming additions from the function name.
            cleanFunctionNameFromCompilerInfo(calledFunctionName);

            // Check if the called function is part of the user input functions list.
            if (std::find(userInputFunctions.begin(), userInputFunctions.end(), calledFunctionName) == userInputFunctions.end())
            {
                // If not, return and do not process further.
                return;
            }

            // Initialize the entry in the instruction dependency map for this call instruction if it doesn't exist.
            if (dependentInstructions.find(callInstruction) == dependentInstructions.end())
            {
                dependentInstructions[callInstruction] = {};
                calledFunc[callInstruction] = calledFunctionName;
            }

            // Collect users of this call instruction recursively.
            defUseAnalysis(callInstruction, dependentInstructions[callInstruction]);

            // Iterate over the arguments of the call instruction.
            for (auto arg = callInstruction->arg_begin(); arg != callInstruction->arg_end(); ++arg)
            {
                if (arg->get()->getType()->getTypeID() == Type::TypeID::PointerTyID)
                {
                    auto argumentValue = arg->get();
                    // Collect users of this argument recursively.
                    defUseAnalysis(argumentValue, dependentInstructions[callInstruction]);
                }
            }

            // Trace the instruction for variable name mapping.
            Instruction *traceInstruction = callInstruction;

            // Iterate over the value to variable name map.
            for (auto entry : variables)
            {
                // Check for Store instructions and update mapping.
                if (auto storeInstruction = dyn_cast<StoreInst>(entry.first))
                {
                    if (storeInstruction->getOperand(0) == traceInstruction)
                    {
                        variables[callInstruction] = variables[entry.first];
                        break;
                    }
                }
                // Check for Cast instructions and update the trace instruction.
                else if (auto asCastInstruction = dyn_cast<CastInst>(entry.first))
                {
                    if (asCastInstruction->getOperand(0) == callInstruction)
                    {
                        traceInstruction = asCastInstruction;
                    }
                }
            }
        }

        void cleanFunctionNameFromCompilerInfo(std::string &functionName)
        {
            // static const std::vector<std::string> compilerInfoToRemove = {"__isoc90_", "__isoc99_"};

            // for (const auto &info : compilerInfoToRemove)
            // {
            //     size_t pos = functionName.find(info);
            //     if (pos != std::string::npos)
            //     {
            //         functionName.erase(pos, info.length());
            //     }
            // }
        }

        void analyzeAndPrintOutput()
        {
            // Iterate over each conditional branch instruction.
            for (auto branch : branches)
            {
                // Retrieve the values involved in the condition of the branch.
                auto condValues = getConditionalValues(branch);
                // Map call instructions to variable names based on these conditional values.
                auto callInstToVarNames = mapCallInstToVarNames(condValues);

                // If there are any call instructions associated with this branch, process them.
                if (!callInstToVarNames.empty())
                {
                    outs() << "\nLine " << branch->getDebugLoc()->getLine() << ": ";
                    writeToFile("\nLine " + std::to_string(branch->getDebugLoc()->getLine()) + ": ");
                    processUserInputCalls(callInstToVarNames, calledFunc);
                }
            }
        }

        std::vector<Value *> getConditionalValues(BranchInst *branch)
        {
            std::vector<Value *> condValues;
            // Get the condition of the branch.
            auto branchCondition = branch->getCondition();
            // If the condition is an integer comparison, collect the values involved in the comparison.
            if (isa<ICmpInst>(branchCondition))
            {
                auto asICmpInst = dyn_cast<ICmpInst>(branchCondition);
                condValues.push_back(asICmpInst);
                condValues.push_back(asICmpInst->getOperand(0));
                condValues.push_back(asICmpInst->getOperand(1));
            }
            return condValues;
        }

        std::map<Value *, std::vector<std::string>> mapCallInstToVarNames(
            const std::vector<Value *> &condValues)
        {
            std::map<Value *, std::vector<std::string>> callInstToVarNames;
            // Iterate over the call instruction dependency map.
            for (auto entry : dependentInstructions)
            {
                // For each instruction that uses a value involved in a conditional branch.
                for (auto use : entry.second)
                {
                    if (std::find(condValues.begin(), condValues.end(), use) != condValues.end())
                    {
                        // If a variable name is associated with the use, add it to the map.
                        if (variables.find(use) != variables.end())
                        {
                            callInstToVarNames[entry.first].push_back(variables.at(use));
                        }
                    }
                }
            }
            return callInstToVarNames;
        }

        void processUserInputCalls(const std::map<Value *, std::vector<std::string>> &callInstToVarNames,
                                   const std::map<Value *, std::string> &calledFunc)
        {
            // Iterate over all call instructions that are influenced by user input.
            for (auto userInputCall : callInstToVarNames)
            {
                // Get the name of the function being called.
                auto calledFunctName = getFunctionName(userInputCall.first);
                // Clean any compiler-specific information from the function name.
                cleanFunctionNameFromCompilerInfo(calledFunctName);

                // Process this call instruction if it's a user input call.
                if (auto asCallInst = dyn_cast<CallInst>(userInputCall.first))
                {
                    processSingleUserInputCall(asCallInst, userInputCall.second, calledFunctName);
                }
            }
        }

        void processSingleUserInputCall(CallInst *asCallInst,
                                        const std::vector<std::string> &variableValues,
                                        const std::string &functionName)
        {
            // Output the detected seminal input and the function causing it.
            writeToFile("\nSeminal input detected: ");
            for (auto var : variableValues)
            {
                writeToFile(var + ", ");
            }
            writeToFile("\n");
            writeToFile("user input using function " + functionName + " on line " + std::to_string(asCallInst->getDebugLoc()->getLine()) + "\n");
        }

        std::string getFunctionName(Value *callInst)
        {
            // Return the function name if found, otherwise return a placeholder.
            return calledFunc.find(callInst) != calledFunc.end() ? calledFunc.at(callInst) : "some function";
        }

        void writeToFile(std::string content)
        {
            std::ofstream file;
            // Delete file if it already exists, otherwise create a new file.
            if (!flag_for_file)
            {
                file.open("lol.txt", std::fstream::out | std::fstream::trunc);
                flag_for_file = true;
            }
            // Open the file in append mode (after start)
            else
            {
                file.open("lol.txt", std::fstream::app);
            }
            file << content;
            file.close();
        }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo()
{
    return {
        .APIVersion = LLVM_PLUGIN_API_VERSION,
        .PluginName = "SeminalInputFeaturesAnalysis",
        .PluginVersion = "v0.1",
        .RegisterPassBuilderCallbacks = [](PassBuilder &PB)
        {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level)
                {
                    MPM.addPass(SeminalInputAnalysis());
                });
        }};
}