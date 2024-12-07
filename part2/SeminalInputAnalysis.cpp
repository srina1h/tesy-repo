#include <string>
#include <vector>
#include <map>
#include <set>
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
                        if (auto *BI = dyn_cast<BranchInst>(&I))
                        {
                            if (BI->isConditional())
                            {
                                branches.push_back(BI);
                            }
                        }
                        else if (auto *DI = dyn_cast<DbgDeclareInst>(&I))
                        {
                            std::string varName = DI->getVariable()->getName().str();

                            // Map the address of the variable to its name.
                            variables[DI->getAddress()] = varName;

                            // Collect all instructions that use this variable.
                            std::set<Instruction *> dependents;
                            defUseAnalysis(DI->getAddress(), dependents);

                            // Iterate over all users of the variable.
                            for (auto dep : dependents)
                            {
                                // if load instruction uses this var in an operand, store the variable name with its address as key
                                if (auto LI = dyn_cast<LoadInst>(dep))
                                {
                                    if (LI->getPointerOperand() == DI->getAddress())
                                    {
                                        variables[LI] = varName;
                                    }
                                }
                                else if (auto UI = dyn_cast<UnaryInstruction>(dep))
                                {
                                    // unary only has 1 operand
                                    variables[UI] = varName;
                                }
                                else if (auto SI = dyn_cast<StoreInst>(dep))
                                {
                                    // If store instruction uses this var in an operand, store the variable name with its address as key
                                    if (SI->getValueOperand() == DI->getAddress())
                                    {
                                        variables[SI] = varName;
                                    }
                                }
                            }
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
                        if (auto CI = dyn_cast<CallInst>(&I))
                        {
                            if (CI->getCalledFunction())
                            {
                                processCallInstruction(CI);
                            }
                        }
                    }
                }
            }

            analyzeAndPrintOutput();

            return PreservedAnalyses::all();
        }

    private:
        /**
         * @brief Define the variables that will be used throughout the analysis.
         *
         * The variables are:
         * 1. flag_for_file: A flag to check if the file has been created or not.
         * 2. variables: Stores the variables mapped to their names
         * 3. dependentInstructions: Mapped instructions to their dependent instructions.
         * 4. branches: All conditional branches are stored here
         * 5. calledFunc: Any function that may be called by user input functions
         */

        bool flag_for_file = false;

        std::map<Value *, std::string> variables;
        std::map<Value *, std::set<Instruction *>> dependentInstructions;
        std::vector<BranchInst *> branches;
        std::map<Value *, std::string> calledFunc;
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

        void defUseAnalysis(Value *value, std::set<Instruction *> &dependents)
        {
            // Check if the value is null to prevent processing an invalid value.
            if (!value)
            {
                return;
            }

            // Iterate over all users of the value.
            for (User *U : value->users())
            {
                // Cast the user to an Instruction, if possible.
                if (Instruction *I = dyn_cast<Instruction>(U))
                {
                    // Avoid re-processing an instruction already in the set.
                    if (dependents.find(I) != dependents.end())
                    {
                        return;
                    }

                    // Add the instruction to the set of users.
                    dependents.insert(I);

                    // Special handling for Store instructions: recursively collect users of the pointer operand.
                    if (isa<StoreInst>(I))
                    {
                        auto storeInst = dyn_cast<StoreInst>(I);
                        defUseAnalysis(storeInst->getPointerOperand(), dependents);
                    }
                    // Special handling for Return instructions: if it returns a value, recursively collect users.
                    else if (isa<ReturnInst>(I))
                    {
                        auto asRetInstruction = dyn_cast<ReturnInst>(I);
                        if (asRetInstruction->getReturnValue())
                        {
                            defUseAnalysis(asRetInstruction->getFunction(), dependents);
                        }
                    }

                    // Recursively collect users of this instruction.
                    defUseAnalysis(I, dependents);
                }
            }
        }

        void processCallInstruction(CallInst *CI)
        {
            std::string FuncName = CI->getCalledFunction()->getName().str();

            // check if function among library functions and if it is, return
            if (std::find(userInputFunctions.begin(), userInputFunctions.end(), FuncName) == userInputFunctions.end())
            {
                return;
            }

            // check if the function is in the calledFuncMap, if its not, add it
            if (dependentInstructions.find(CI) == dependentInstructions.end())
            {
                dependentInstructions[CI] = std::set<Instruction *>();
                calledFunc[CI] = FuncName;
            }

            // Collect users of this call instruction recursively.
            defUseAnalysis(CI, dependentInstructions[CI]);

            // Iterate over the arguments of the call instruction.
            for (auto arg = CI->arg_begin(); arg != CI->arg_end(); ++arg)
            {
                if (arg->get()->getType()->getTypeID() == Type::TypeID::PointerTyID)
                {
                    auto argumentValue = arg->get();
                    // Collect users of this argument recursively.
                    defUseAnalysis(argumentValue, dependentInstructions[CI]);
                }
            }

            // Trace the instruction for variable name mapping.
            Instruction *nextInst = CI;

            // Iterate over the value to variable name map.
            for (auto var : variables)
            {
                // Check for Store instructions and update mapping.
                if (auto SI = dyn_cast<StoreInst>(var.first))
                {
                    if (SI->getOperand(0) == nextInst)
                    {
                        variables[CI] = variables[var.first];
                        break;
                    }
                }
                // Check for Cast instructions and update the trace instruction.
                else if (auto CAI = dyn_cast<CastInst>(var.first))
                {
                    if (CAI->getOperand(0) == CI)
                    {
                        nextInst = CAI;
                    }
                }
            }
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
                // cleanFunctionNameFromCompilerInfo(calledFunctName);

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