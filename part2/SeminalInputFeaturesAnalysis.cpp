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
         *
         * This function is the main function of the pass. It is called by the pass manager to run the analysis.
         * The function performs the following tasks:
         * 1. Find conditional branching and store them
         * 2. Map the variables to their names (using debug information)
         * 3. Find the ways in which the mapped variables are used
         * 4. Perform the analysis and print the outputs to a file
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

        /**
         * @brief Perform def-use analysis on the given value(instruction) and find its dependent instructions
         * @param v The instruction to be analyzed
         * @param dependents The set of dependent instructions
         *
         * This function performs def-use analysis on the given instruction and finds its dependent instructions.
         * It recursively calls itself on the dependent instructions to find their dependent instructions.
         * Once we have the dependent instructions mapped, we are able to use to find the ways in which the variables are used.
         */
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

        /**
         * @brief For call instructions that are user input functions, find the ways in which the mapped variables are used
         * @param CI The call instruction to be analyzed
         *
         * This function performs the following tasks:
         * 1. Check if the function is among the library functions
         * 2. Find the dependent instructions of the call instruction
         * 3. Perform def-use analysis on the arguments of the function that is being called (if call by reference)
         * 4. Trace the instruction for variable name mapping
         */
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

            // Perform def-use analysis to find the dependent instructions
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

            Instruction *nextInst = CI;

            /* Go through previously mapped variables and check if they are instructions
            that are dependent on the current call instruction. if so, update the variables map
            to reflect that the current call instuction is responsible for the use of the variable
            */

            for (auto var : variables)
            {
                if (auto SI = dyn_cast<StoreInst>(var.first))
                {
                    if (SI->getOperand(0) == nextInst)
                    {
                        variables[CI] = variables[var.first];
                        break;
                    }
                }
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