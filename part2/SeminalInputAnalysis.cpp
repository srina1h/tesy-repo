#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/Module.h"

#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include <list>
#include <string>

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

            std::map<llvm::Value *, std::set<llvm::Instruction *>> callInstructionDependencyMap;
            // Maps a call instruction to a set of instructions that are dependent on it.
            // This helps in tracking the flow and influence of function calls throughout the program.

            std::list<BranchInst *> branches;
            // A list of all conditional branch instructions found during the analysis.
            // These are the branches that will be analyzed for user input dependencies.

            std::map<llvm::Value *, std::string> valueToNameMap;
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
                                processBranchInst(branchInst, branches);
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
                                processCallInstruction(asCallInst, callInstructionDependencyMap, valueToNameMap, variables);
                            }
                        }
                    }
                }
            }

            analyzeAndPrintOutput(variables, valueToNameMap, callInstructionDependencyMap, branches);

            return PreservedAnalyses::all();
        }

    private:
        // Process debug information to map variables
        void processDbgInst(llvm::DbgDeclareInst *asDbgInst, std::map<Value *, std::string> &valueToVariableNameMap);

        // Recursively collect user instructions that depend on a value
        void collectUsersRecursive(Value *value, std::set<Instruction *> &userInstructions);

        // Process call instructions to track dependencies
        void processCallInstruction(llvm::CallInst *asCallInst, std::map<Value *, std::set<Instruction *>> &callInstructionDependencyMap,
                                    std::map<llvm::Value *, std::string> &valueToNameMap,
                                    std::map<Value *, std::string> &valueToVariableNameMap);

        // Clean function names from compiler-added information
        void cleanFunctionNameFromCompilerInfo(std::string &functionName);

        // Analyze the module and print output related to seminal input
        void analyzeAndPrintOutput(std::map<llvm::Value *, std::string> &valueToVariableNameMap,
                                   std::map<llvm::Value *, std::string> &valueToNameMap,
                                   std::map<llvm::Value *, std::set<llvm::Instruction *>> &callInstructionDependencyMap,
                                   std::list<llvm::BranchInst *> &conditionalBranches);

        // Get conditional values for a branch instruction
        std::list<llvm::Value *> getConditionalValues(llvm::BranchInst *branch);

        // Map call instructions to variable names influenced by user input
        std::map<llvm::Value *, std::vector<std::string>> mapCallInstToVarNames(
            const std::list<llvm::Value *> &condValues,
            const std::map<llvm::Value *, std::set<llvm::Instruction *>> &callInstructionDependencyMap,
            const std::map<llvm::Value *, std::string> &valueToVariableNameMap);

        // Process calls related to user input
        void processUserInputCalls(const std::map<llvm::Value *, std::vector<std::string>> &callInstToVarNames,
                                   const std::map<llvm::Value *, std::string> &valueToNameMap);

        // Process a single user input call
        void processSingleUserInputCall(llvm::CallInst *asCallInst,
                                        const std::vector<std::string> &variableValues,
                                        const std::string &functionName);

        // Get function name from a call instruction
        std::string getFunctionName(llvm::Value *callInst, const std::map<llvm::Value *, std::string> &valueToNameMap);

        // Utility to join strings with a delimiter
        std::string join(const std::vector<std::string> &vec, const std::string &delim);

        // Get the line number from a basic block
        int getLine(llvm::BasicBlock &BB);

        // Process branch instructions to collect branch info and conditional branches
        void processBranchInst(llvm::BranchInst *branchInst,
                               std::list<llvm::BranchInst *> &conditionalBranches);

        void writeToFile(std::string content);
    };

    void SeminalInputAnalysis::processDbgInst(DbgDeclareInst *asDbgInst, std::map<Value *, std::string> &valueToVariableNameMap)
    {
        // Extract the variable name from the debug information.
        std::string variableName = asDbgInst->getVariable()->getName().str();

        // Map the address of the variable to its name.
        valueToVariableNameMap[asDbgInst->getAddress()] = variableName;

        // Collect all instructions that use this variable.
        std::set<Instruction *> varUsers;
        collectUsersRecursive(asDbgInst->getAddress(), varUsers);

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

        // write the valueToVariableNameMap to the file
        std::string va = "valueToVariableNameMap\n";
        for (auto entry : valueToVariableNameMap)
        {
            va += entry.second + " : " + std::to_string(reinterpret_cast<uintptr_t>(entry.first)) + "\n";
        }

        writeToFile(va);
    }

    void SeminalInputAnalysis::collectUsersRecursive(Value *value, std::set<Instruction *> &userInstructions)
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
                    collectUsersRecursive(storeInst->getPointerOperand(), userInstructions);
                }
                // Special handling for Return instructions: if it returns a value, recursively collect users.
                else if (isa<ReturnInst>(inst))
                {
                    auto asRetInstruction = dyn_cast<ReturnInst>(inst);
                    if (asRetInstruction->getReturnValue())
                    {
                        collectUsersRecursive(asRetInstruction->getFunction(), userInstructions);
                    }
                }

                // Recursively collect users of this instruction.
                collectUsersRecursive(inst, userInstructions);
            }
        }
    }

    void SeminalInputAnalysis::processCallInstruction(CallInst *callInstruction,
                                                      std::map<Value *, std::set<Instruction *>> &instructionDependencyMap,
                                                      std::map<llvm::Value *, std::string> &valueToNameMap,
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
        collectUsersRecursive(callInstruction, instructionDependencyMap[callInstruction]);

        // Iterate over the arguments of the call instruction.
        for (auto arg = callInstruction->arg_begin(); arg != callInstruction->arg_end(); ++arg)
        {
            if (arg->get()->getType()->getTypeID() == llvm::Type::TypeID::PointerTyID)
            {
                auto argumentValue = arg->get();
                // Collect users of this argument recursively.
                collectUsersRecursive(argumentValue, instructionDependencyMap[callInstruction]);
            }
        }

        // Trace the instruction for variable name mapping.
        llvm::Instruction *traceInstruction = callInstruction;

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

    void SeminalInputAnalysis::cleanFunctionNameFromCompilerInfo(std::string &functionName)
    {
        static const std::vector<std::string> compilerInfoToRemove = {"__isoc90_", "__isoc99_"};

        for (const auto &info : compilerInfoToRemove)
        {
            size_t pos = functionName.find(info);
            if (pos != std::string::npos)
            {
                functionName.erase(pos, info.length());
            }
        }
    }

    void SeminalInputAnalysis::analyzeAndPrintOutput(std::map<llvm::Value *, std::string> &valueToVariableNameMap,
                                                     std::map<llvm::Value *, std::string> &valueToNameMap,
                                                     std::map<llvm::Value *, std::set<llvm::Instruction *>> &callInstructionDependencyMap,
                                                     std::list<llvm::BranchInst *> &conditionalBranches)
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
                llvm::outs() << "\nLine " << branch->getDebugLoc()->getLine() << ": ";
                processUserInputCalls(callInstToVarNames, valueToNameMap);
            }
        }
    }

    std::list<llvm::Value *> SeminalInputAnalysis::getConditionalValues(llvm::BranchInst *branch)
    {
        std::list<llvm::Value *> condValues;
        // Get the condition of the branch.
        auto branchCondition = branch->getCondition();
        // If the condition is an integer comparison, collect the values involved in the comparison.
        if (llvm::isa<llvm::ICmpInst>(branchCondition))
        {
            auto asICmpInst = llvm::dyn_cast<llvm::ICmpInst>(branchCondition);
            condValues.emplace_back(asICmpInst);
            condValues.emplace_back(asICmpInst->getOperand(0));
            condValues.emplace_back(asICmpInst->getOperand(1));
        }
        return condValues;
    }

    std::map<llvm::Value *, std::vector<std::string>> SeminalInputAnalysis::mapCallInstToVarNames(
        const std::list<llvm::Value *> &condValues,
        const std::map<llvm::Value *, std::set<llvm::Instruction *>> &callInstructionDependencyMap,
        const std::map<llvm::Value *, std::string> &valueToVariableNameMap)
    {
        std::map<llvm::Value *, std::vector<std::string>> callInstToVarNames;
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

            // std::string va = "callInstToVarNames\n";
            // for (auto entry : callInstToVarNames)
            // {
            //     va += std::to_string(reinterpret_cast<uintptr_t>(entry.first)) + " : ";
            //     for (auto ins : entry.second)
            //     {
            //         va += ins + " ";
            //     }
            //     va += "\n";
            // }

            // writeToFile(va);
        }
        return callInstToVarNames;
    }

    void SeminalInputAnalysis::processUserInputCalls(const std::map<llvm::Value *, std::vector<std::string>> &callInstToVarNames,
                                                     const std::map<llvm::Value *, std::string> &valueToNameMap)
    {
        // Iterate over all call instructions that are influenced by user input.
        for (auto userInputCall : callInstToVarNames)
        {
            // Get the name of the function being called.
            auto calledFunctName = getFunctionName(userInputCall.first, valueToNameMap);
            // Clean any compiler-specific information from the function name.
            cleanFunctionNameFromCompilerInfo(calledFunctName);

            // Process this call instruction if it's a user input call.
            if (auto asCallInst = llvm::dyn_cast<llvm::CallInst>(userInputCall.first))
            {
                processSingleUserInputCall(asCallInst, userInputCall.second, calledFunctName);
            }
        }
    }

    void SeminalInputAnalysis::processSingleUserInputCall(llvm::CallInst *asCallInst,
                                                          const std::vector<std::string> &variableValues,
                                                          const std::string &functionName)
    {
        // Join the variable names influenced by this call into a single string.
        std::string varUseString = join(variableValues, ", ");
        // Output the detected seminal input and the function causing it.
        // write this to a file
        llvm::outs() << "\n\tSeminal input detected: " << varUseString << "\n";
        llvm::outs() << "\tuser input using function " << functionName << " on line " << asCallInst->getDebugLoc()->getLine() << "\n";
        llvm::outs().flush();
    }

    std::string SeminalInputAnalysis::getFunctionName(llvm::Value *callInst, const std::map<llvm::Value *, std::string> &valueToNameMap)
    {
        // Return the function name if found, otherwise return a placeholder.
        return valueToNameMap.find(callInst) != valueToNameMap.end() ? valueToNameMap.at(callInst) : "some function";
    }

    void SeminalInputAnalysis::writeToFile(std::string content)
    {
        // Write the content to a file lol.txt, if not there, create one
        std::ofstream file;
        file.open("lol.txt", std::fstream::app);
        file << content;
        file.close();
    }

    // Utility function to join elements of a vector into a string separated by a delimiter.
    std::string SeminalInputAnalysis::join(const std::vector<std::string> &vec, const std::string &delim)
    {
        std::ostringstream oss;
        // Concatenate the elements of the vector, separated by the delimiter.
        for (size_t i = 0; i < vec.size(); ++i)
        {
            if (i != 0)
                oss << delim;
            oss << vec[i];
        }
        return oss.str();
    }

    int SeminalInputAnalysis::getLine(BasicBlock &BB)
    {
        if (!BB.empty())
        {
            for (Instruction &I : BB)
            {
                DebugLoc DL = I.getDebugLoc();
                if (DL)
                {
                    return DL.getLine();
                }
            }
        }
        return -1;
    }

    void SeminalInputAnalysis::processBranchInst(BranchInst *branchInst,
                                                 std::list<BranchInst *> &conditionalBranches)
    {
        // Retrieve the debug location of the branch instruction.
        DebugLoc DL = branchInst->getDebugLoc();
        // If no debug information is present, skip processing this instruction.
        if (!DL)
            return;

        // Get the scope (e.g., file) in which the branch instruction is located.
        DIScope *scope = cast<DIScope>(DL.getScope());
        // Get the line number of the branch instruction.
        int line = DL.getLine();

        // Iterate over all successors of the branch instruction.
        for (unsigned i = 0; i < branchInst->getNumSuccessors(); ++i)
        {
            BasicBlock *succ = branchInst->getSuccessor(i);
            int targetLine = getLine(*succ);
            // Create a unique identifier for the branch (using the name or address).
            std::string brID = succ->hasName() ? succ->getName().str() : "br_" + std::to_string(reinterpret_cast<uintptr_t>(succ));
            // Store the branch information in the map.
        }
        // Add the branch instruction to the list of conditional branches.
        conditionalBranches.emplace_back(branchInst);
    }

}

extern "C" LLVM_ATTRIBUTE_WEAK PassPluginLibraryInfo llvmGetPassPluginInfo()
{
    return {
        LLVM_PLUGIN_API_VERSION,
        "SeminalInputAnalysis",
        "v0.1",
        [](PassBuilder &PB)
        {
            // Register your pass to run at the end of the module optimization pipeline
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level)
                {
                    MPM.addPass(SeminalInputAnalysis());
                });
        }};
}
