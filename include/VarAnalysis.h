#ifndef VARANALYSIS
#define VARANALYSIS

#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <math.h>
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/Metadata.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/TypeFinder.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/Operator.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Demangle/Demangle.h"

#include "../include/PTA.h"

using namespace llvm;

namespace mqttactic
{
    struct NamedField
    {
        llvm::Type *type;
        const llvm::DIType *typeMD;
        std::string fieldName;
        int typeID;
    };

    struct NamedStructType
    {
        llvm::StructType *type;
        const llvm::DIType *typeMD;
        std::string typeName;
        std::vector<NamedField *> fields;
    };

    struct KeyVariable
    {
        std::string name;
        bool completed = 0;
    };

    class VarAnalysis
    {
    public:
        // struct/class type
        std::vector<NamedStructType *> NamedStructTypes;
        // static/global variables
        std::map<std::string, const Metadata *> GlobalVars;
        // {"class:key_var":[Basicblock*, semantics]}
        std::map<KeyVariable *, std::set<mqttactic::SemanticKBB *>> SemanticKeyBasicBlocks;
        mqttactic::PTA *PointerAnalyzer;

        VarAnalysis(Module &M)
        {
            this->PointerAnalyzer = new mqttactic::PTA(M);
            DebugInfoFinder *dbgFinder = new DebugInfoFinder();
            dbgFinder->processModule(M);
            std::vector<llvm::StructType *> struct_set;
            struct_set = M.getIdentifiedStructTypes();
            for (std::vector<llvm::StructType *>::iterator sit = struct_set.begin(); sit != struct_set.end(); sit++)
            {
                mqttactic::NamedStructType *named_struct = new mqttactic::NamedStructType();
                NamedStructTypes.push_back(named_struct);
                named_struct->type = (*sit);
                named_struct->typeName = (*sit)->getName().str();
                // dbgs() << named_struct->typeName << "\n";
                for (auto *element_type : (*sit)->elements())
                {
                    mqttactic::NamedField *named_field = new mqttactic::NamedField();
                    named_field->type = element_type;
                    named_field->typeID = element_type->getTypeID();
                    named_struct->fields.insert(named_struct->fields.end(), named_field);
                    // dbgs() << named_field->typeID << "\n";
                }

                GetStructDbgInfo(M, dbgFinder, named_struct);
            }
            for (auto global_var : dbgFinder->global_variables())
            {
                const auto *GV = global_var->getVariable();
                if (!GV->getLinkageName().empty())
                {
                    GlobalVars.insert(std::pair<std::string, const llvm::Metadata *>(GV->getLinkageName().str(), GV));
                }
            }
        }

        std::string getBBLabel(const BasicBlock *Node);
        void PrintDbgInfo();
        const DIType *GetBasicDIType(const Metadata *MD);
        std::string GetScope(const DIType *MD);
        void GetStructDbgInfo(Module &M, DebugInfoFinder *dbgFinder, NamedStructType *named_struct);
        llvm::GlobalVariable *GetStaticDbgInfo(Module &M, DIDerivedType *static_var);
        void SearchKeyVar(Module &M, Function &F, std::vector<KeyVariable *> &key_variables);
        bool ParseVariables(Value *V, Module &M, const Function &F, std::string key_var);
    };
}

#endif