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
    class SeminalInputFeaturesAnalysis : public PassInfoMixin<SeminalInputFeaturesAnalysis>
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
            writeToFile("Seminal Input Features Analysis Pass:\n");
            writeToFile("====================================\n");

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
                                checkCallInst(CI);
                            }
                        }
                    }
                }
            }

            performAnalysis();

            writeToFile("\n====================================\n");
            writeToFile("End of Seminal Input Features Analysis Pass\n");

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
        std::vector<std::string> libraryFunctions = {"getc", "fopen", "scanf", "fclose", "fread", "fwrite"};

        void defUseAnalysis(Value *v, std::set<Instruction *> &dependents)
        {
            // Check for value
            if (!v)
            {
                return;
            }

            for (User *U : v->users())
            {
                if (Instruction *I = dyn_cast<Instruction>(U))
                {
                    // dont redo work
                    if (dependents.find(I) != dependents.end())
                    {
                        return;
                    }

                    // add this instruction to set of dependents
                    dependents.insert(I);

                    // recursively call the defUseAnalysis function on the pointer operand of the store instruction
                    if (StoreInst *SI = dyn_cast<StoreInst>(I))
                    {
                        defUseAnalysis(SI->getPointerOperand(), dependents);
                    }
                    // recursively call the defUseAnalysis function on the return value of the return instruction
                    else if (ReturnInst *RI = dyn_cast<ReturnInst>(I))
                    {
                        if (RI->getReturnValue())
                        {
                            defUseAnalysis(RI->getFunction(), dependents);
                        }
                    }
                    else
                    {
                        // recursively call defUseAnalysis
                        defUseAnalysis(I, dependents);
                    }
                }
            }
        }

        void checkCallInst(CallInst *CI)
        {
            std::string FuncName = CI->getCalledFunction()->getName().str();

            // check if function among library functions and if it is, return
            if (std::find(libraryFunctions.begin(), libraryFunctions.end(), FuncName) == libraryFunctions.end())
            {
                return;
            }

            // check if the function is in the dependentInstructions, if its not, add it
            if (dependentInstructions.find(CI) == dependentInstructions.end())
            {
                dependentInstructions[CI] = std::set<Instruction *>();
                calledFunc[CI] = FuncName;
            }

            // Collect users of this call instruction recursively.
            defUseAnalysis(CI, dependentInstructions[CI]);

            // Perform def-use analysis on the arguments of the function
            for (auto arg = CI->arg_begin(); arg != CI->arg_end(); ++arg)
            {
                // only perform analysis on pointer arguments (others maybe call by value)
                if (arg->get()->getType()->getTypeID() == Type::TypeID::PointerTyID)
                {
                    auto argumentValue = arg->get();
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

        void performAnalysis()
        {
            for (auto branch : branches)
            {
                auto condition = branch->getCondition();

                if (isa<ICmpInst>(condition))
                {
                    std::map<Value *, std::vector<std::string>> branch_call_mapping;
                    auto cmp = dyn_cast<ICmpInst>(condition);
                    auto op1 = cmp->getOperand(0);
                    auto op2 = cmp->getOperand(1);

                    for (auto inst : dependentInstructions)
                    {
                        for (auto dep : inst.second)
                        {
                            if (dep == op1 || dep == op2 || dep == cmp)
                            {
                                if (variables.find(dep) != variables.end())
                                {
                                    branch_call_mapping[inst.first].push_back(variables.at(dep));
                                }
                            }
                        }
                    }

                    if (branch_call_mapping.size() > 0)
                    {
                        writeToFile("\nLine " + std::to_string(branch->getDebugLoc()->getLine()) + ": ");

                        for (auto call : branch_call_mapping)
                        {
                            std::string funcName = calledFunc.find(call.first) != calledFunc.end() ? calledFunc.at(call.first) : "unkown";

                            if (auto CI = dyn_cast<CallInst>(call.first))
                            {
                                writeToFile("\nSeminal input detected: ");
                                for (auto var : call.second)
                                {
                                    writeToFile(var + ", ");
                                }
                                writeToFile("\n");
                                writeToFile("user input using function " + funcName + " on line " + std::to_string(CI->getDebugLoc()->getLine()) + "\n");
                            }
                        }
                    }
                }
            }
        }

        // void perform_Analysis()
        // {
        //     // Iterate over each conditional branch instruction.
        //     for (auto branch : branches)
        //     {
        //         // Retrieve the values involved in the condition of the branch.
        //         auto condValues = getConditionalValues(branch);
        //         // Map call instructions to variable names based on these conditional values.
        //         auto callInstToVarNames = mapCallInstToVarNames(condValues);

        //         // If there are any call instructions associated with this branch, process them.
        //         if (!callInstToVarNames.empty())
        //         {
        //             writeToFile("\nLine " + std::to_string(branch->getDebugLoc()->getLine()) + ": ");
        //             processUserInputCalls(callInstToVarNames, calledFunc);
        //         }
        //     }
        // }

        // std::vector<Value *> getConditionalValues(BranchInst *branch)
        // {
        //     std::vector<Value *> condValues;
        //     // Get the condition of the branch.
        //     auto branchCondition = branch->getCondition();
        //     // If the condition is an integer comparison, collect the values involved in the comparison.
        //     if (isa<ICmpInst>(branchCondition))
        //     {
        //         auto asICmpInst = dyn_cast<ICmpInst>(branchCondition);
        //         condValues.push_back(asICmpInst);
        //         condValues.push_back(asICmpInst->getOperand(0));
        //         condValues.push_back(asICmpInst->getOperand(1));
        //     }
        //     return condValues;
        // }

        // std::map<Value *, std::vector<std::string>> mapCallInstToVarNames(
        //     const std::vector<Value *> &condValues)
        // {
        //     std::map<Value *, std::vector<std::string>> callInstToVarNames;
        //     // Iterate over the call instruction dependency map.
        //     for (auto entry : dependentInstructions)
        //     {
        //         // For each instruction that uses a value involved in a conditional branch.
        //         for (auto use : entry.second)
        //         {
        //             if (std::find(condValues.begin(), condValues.end(), use) != condValues.end())
        //             {
        //                 // If a variable name is associated with the use, add it to the map.
        //                 if (variables.find(use) != variables.end())
        //                 {
        //                     callInstToVarNames[entry.first].push_back(variables.at(use));
        //                 }
        //             }
        //         }
        //     }
        //     return callInstToVarNames;
        // }

        // void processUserInputCalls(const std::map<Value *, std::vector<std::string>> &callInstToVarNames,
        //                            const std::map<Value *, std::string> &calledFunc)
        // {
        //     // Iterate over all call instructions that are influenced by user input.
        //     for (auto userInputCall : callInstToVarNames)
        //     {
        //         // Get the name of the function being called.
        //         std::string calledFunctName = calledFunc.find(userInputCall.first) != calledFunc.end() ? calledFunc.at(userInputCall.first) : "unkown";

        //         // Process this call instruction if it's a user input call.
        //         if (auto asCallInst = dyn_cast<CallInst>(userInputCall.first))
        //         {
        //             processSingleUserInputCall(asCallInst, userInputCall.second, calledFunctName);
        //         }
        //     }
        // }

        // void processSingleUserInputCall(CallInst *asCallInst,
        //                                 const std::vector<std::string> &variableValues,
        //                                 const std::string &functionName)
        // {
        //     // Output the detected seminal input and the function causing it.
        //     writeToFile("\nSeminal input detected: ");
        //     for (auto var : variableValues)
        //     {
        //         writeToFile(var + ", ");
        //     }
        //     writeToFile("\n");
        //     writeToFile("user input using function " + functionName + " on line " + std::to_string(asCallInst->getDebugLoc()->getLine()) + "\n");
        // }

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
                    MPM.addPass(SeminalInputFeaturesAnalysis());
                });
        }};
}