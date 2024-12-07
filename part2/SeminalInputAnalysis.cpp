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
                            processDbgInst(asDbgInst, variables);
                        }
                    }
                }
            }
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
                                processCallInstruction(asCallInst, dependentInstructions, calledFunc, variables);
                            }
                        }
                    }
                }
            }

            analyzeAndPrintOutput(variables, calledFunc, dependentInstructions, branches);

            return PreservedAnalyses::all();
        }

    private:
        bool flag_for_file = false;

        void processDbgInst(DbgDeclareInst *asDbgInst, std::map<Value *, std::string> &valueToVariableNameMap)
        {
            // Extract the variable name from the debug information.
            std::string variableName = asDbgInst->getVariable()->getName().str();

            // Map the address of the variable to its name.
            valueToVariableNameMap[asDbgInst->getAddress()] = variableName;

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
                        valueToVariableNameMap[loadInst] = variableName;
                    }
                }
                // Similarly, for Store instructions, map the instruction to the variable name.
                else if (auto storeInst = dyn_cast<StoreInst>(user))
                {
                    if (storeInst->getOperand(1) == asDbgInst->getAddress())
                    {
                        valueToVariableNameMap[storeInst] = variableName;
                    }
                }
                // For Unary instructions, directly map the instruction to the variable name.
                else if (auto unaryInst = dyn_cast<UnaryInstruction>(user))
                {
                    valueToVariableNameMap[unaryInst] = variableName;
                }
            }
        }

        void defUseAnalysis(Value *value, std::set<Instruction *> &userInstructions)
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
                    if (userInstructions.find(inst) != userInstructions.end())
                    {
                        return;
                    }

                    // Add the instruction to the set of users.
                    userInstructions.insert(inst);

                    // Special handling for Store instructions: recursively collect users of the pointer operand.
                    if (isa<StoreInst>(inst))
                    {
                        auto storeInst = dyn_cast<StoreInst>(inst);
                        defUseAnalysis(storeInst->getPointerOperand(), userInstructions);
                    }
                    // Special handling for Return instructions: if it returns a value, recursively collect users.
                    else if (isa<ReturnInst>(inst))
                    {
                        auto asRetInstruction = dyn_cast<ReturnInst>(inst);
                        if (asRetInstruction->getReturnValue())
                        {
                            defUseAnalysis(asRetInstruction->getFunction(), userInstructions);
                        }
                    }

                    // Recursively collect users of this instruction.
                    defUseAnalysis(inst, userInstructions);
                }
            }
        }

        void processCallInstruction(CallInst *callInstruction,
                                    std::map<Value *, std::set<Instruction *>> &instructionDependencyMap,
                                    std::map<Value *, std::string> &valueToNameMap,
                                    std::map<Value *, std::string> &valueToVariableNameMap)
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
            if (instructionDependencyMap.find(callInstruction) == instructionDependencyMap.end())
            {
                instructionDependencyMap[callInstruction] = {};
                valueToNameMap[callInstruction] = calledFunctionName;
            }

            // Collect users of this call instruction recursively.
            defUseAnalysis(callInstruction, instructionDependencyMap[callInstruction]);

            // Iterate over the arguments of the call instruction.
            for (auto arg = callInstruction->arg_begin(); arg != callInstruction->arg_end(); ++arg)
            {
                if (arg->get()->getType()->getTypeID() == Type::TypeID::PointerTyID)
                {
                    auto argumentValue = arg->get();
                    // Collect users of this argument recursively.
                    defUseAnalysis(argumentValue, instructionDependencyMap[callInstruction]);
                }
            }

            // Trace the instruction for variable name mapping.
            Instruction *traceInstruction = callInstruction;

            // Iterate over the value to variable name map.
            for (auto entry : valueToVariableNameMap)
            {
                // Check for Store instructions and update mapping.
                if (auto storeInstruction = dyn_cast<StoreInst>(entry.first))
                {
                    if (storeInstruction->getOperand(0) == traceInstruction)
                    {
                        valueToVariableNameMap[callInstruction] = valueToVariableNameMap[entry.first];
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

        void analyzeAndPrintOutput(std::map<Value *, std::string> &valueToVariableNameMap,
                                   std::map<Value *, std::string> &valueToNameMap,
                                   std::map<Value *, std::set<Instruction *>> &callInstructionDependencyMap,
                                   std::vector<BranchInst *> &conditionalBranches)
        {
            // Iterate over each conditional branch instruction.
            for (auto branch : conditionalBranches)
            {
                // Retrieve the values involved in the condition of the branch.
                auto condValues = getConditionalValues(branch);
                // Map call instructions to variable names based on these conditional values.
                auto callInstToVarNames = mapCallInstToVarNames(condValues, callInstructionDependencyMap, valueToVariableNameMap);

                // If there are any call instructions associated with this branch, process them.
                if (!callInstToVarNames.empty())
                {
                    outs() << "\nLine " << branch->getDebugLoc()->getLine() << ": ";
                    writeToFile("\nLine " + std::to_string(branch->getDebugLoc()->getLine()) + ": ");
                    processUserInputCalls(callInstToVarNames, valueToNameMap);
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
            const std::vector<Value *> &condValues,
            const std::map<Value *, std::set<Instruction *>> &callInstructionDependencyMap,
            const std::map<Value *, std::string> &valueToVariableNameMap)
        {
            std::map<Value *, std::vector<std::string>> callInstToVarNames;
            // Iterate over the call instruction dependency map.
            for (auto entry : callInstructionDependencyMap)
            {
                // For each instruction that uses a value involved in a conditional branch.
                for (auto use : entry.second)
                {
                    if (std::find(condValues.begin(), condValues.end(), use) != condValues.end())
                    {
                        // If a variable name is associated with the use, add it to the map.
                        if (valueToVariableNameMap.find(use) != valueToVariableNameMap.end())
                        {
                            callInstToVarNames[entry.first].push_back(valueToVariableNameMap.at(use));
                        }
                    }
                }
            }
            return callInstToVarNames;
        }

        void processUserInputCalls(const std::map<Value *, std::vector<std::string>> &callInstToVarNames,
                                   const std::map<Value *, std::string> &valueToNameMap)
        {
            // Iterate over all call instructions that are influenced by user input.
            for (auto userInputCall : callInstToVarNames)
            {
                // Get the name of the function being called.
                auto calledFunctName = getFunctionName(userInputCall.first, valueToNameMap);
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

        std::string getFunctionName(Value *callInst, const std::map<Value *, std::string> &valueToNameMap)
        {
            // Return the function name if found, otherwise return a placeholder.
            return valueToNameMap.find(callInst) != valueToNameMap.end() ? valueToNameMap.at(callInst) : "some function";
        }

        void writeToFile(std::string content)
        {
            // Write the content to a file lol.txt, if not there, create one, if it alreeady exists delete it

            std::ofstream file;
            if (!flag_for_file)
            {
                file.open("lol.txt", std::fstream::out | std::fstream::trunc);
                flag_for_file = true;
            }
            else
            {
                file.open("lol.txt", std::fstream::app);
            }
            file << content;
            file.close();

            // std::ofstream file;
            // file.open("lol.txt", std::fstream::app);

            // file << content;
            // file.close();
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