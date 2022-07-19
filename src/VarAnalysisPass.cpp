#include <set>
#include <vector>
#include <map>
#include <fstream>
#include <math.h>
#include "llvm/Pass.h"
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

using namespace llvm;

namespace
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

    class VarAnalysis : public ModulePass
    {
    public:
        static char ID;
        std::vector<NamedStructType *> NamedStructTypes;

        const DIType *GetBasicDIType(const Metadata *MD);
        std::string GetScope(const DIType *MD);
        void GetStructDbgInfo(DebugInfoFinder *dbgFinder, NamedStructType *named_struct);
        VarAnalysis() : ModulePass(ID)
        {
        }
        bool runOnModule(Module &M) override
        {
            DebugInfoFinder *dbgFinder = new DebugInfoFinder();
            dbgFinder->processModule(M);
            std::vector<llvm::StructType *> struct_set;
            struct_set = M.getIdentifiedStructTypes();
            for (std::vector<llvm::StructType *>::iterator sit = struct_set.begin(); sit != struct_set.end(); sit++)
            {
                NamedStructType *named_struct = new NamedStructType();
                NamedStructTypes.push_back(named_struct);
                named_struct->type = (*sit);
                named_struct->typeName = (*sit)->getName().str();
                errs() << named_struct->typeName << "\n";
                for (auto *element_type : (*sit)->elements())
                {
                    NamedField *named_field = new NamedField();
                    named_field->type = element_type;
                    named_field->typeID = element_type->getTypeID();
                    named_struct->fields.insert(named_struct->fields.end(), named_field);
                    errs() << named_field->typeID << "\n";
                }

                GetStructDbgInfo(dbgFinder, named_struct);
            }
            errs() << "----------------------------------\n";
            for (auto *named_struct : NamedStructTypes)
            {
                errs() << named_struct->typeName << "\n{\n";
                for (auto *named_field : named_struct->fields)
                {
                    if (named_field->typeMD)
                        errs() << "    " << named_field->fieldName << " : " << GetScope(GetBasicDIType(named_field->typeMD)) << GetBasicDIType(named_field->typeMD)->getName() << "\n";
                }
                errs() << "}\n";
            }

            return false;
        }
    };
}

const DIType *VarAnalysis::GetBasicDIType(const Metadata *MD)
{
    const DIType *ret = nullptr;
    switch (MD->getMetadataID())
    {
    case Metadata::DIBasicTypeKind:
    {
        auto *BT = dyn_cast<DIBasicType>(MD);
        ret = BT;
        break;
    }
    case Metadata::DIDerivedTypeKind:
    {
        auto *DerivedT = dyn_cast<DIDerivedType>(MD);
        ret = DerivedT->getBaseType();
        break;
    }
    case Metadata::DICompositeTypeKind:
    {
        auto *CT = dyn_cast<DICompositeType>(MD);
        ret = CT;
        break;
    }
    default:
        break;
    }

    return ret;
}

std::string VarAnalysis::GetScope(const DIType *MD)
{
    std::string scope = "";
    DIScope *scope_node = MD->getScope();
    while (scope_node != NULL)
    {
        scope = scope_node->getName().str() + "::" + scope;
        scope_node = scope_node->getScope();
    }
    return scope;
}

void VarAnalysis::GetStructDbgInfo(DebugInfoFinder *dbgFinder, NamedStructType *named_struct)
{
    for (const DIType *T : dbgFinder->types())
    {
        if (!T->getName().empty())
        {
            std::string scope_name = GetScope(T) + T->getName().str();

            if (named_struct->typeName.find(scope_name) == std::string::npos || named_struct->typeName.find(scope_name) + scope_name.size() != named_struct->typeName.size())
            {
                continue;
            }

            switch (T->getMetadataID())
            {
            // case Metadata::DIBasicTypeKind:
            // {
            //     auto *BT = dyn_cast<DIBasicType>(T);
            //     auto Encoding = dwarf::AttributeEncodingString(BT->getEncoding());
            //     if (!Encoding.empty())
            //         errs() << Encoding;
            //     else
            //         errs() << "unknown-encoding(" << BT->getEncoding() << ')';
            //     break;
            // }
            // case Metadata::DIDerivedTypeKind:
            // {
            //     auto Tag = dwarf::TagString(T->getTag());
            //     if (!Tag.empty())
            //         errs() << Tag << "\n";
            //     else
            //         errs() << "unknown-tag(" << T->getTag() << ")\n";
            //     break;
            // }
            case Metadata::DICompositeTypeKind:
            {
                auto *CT = dyn_cast<DICompositeType>(T);
                auto Tag = dwarf::TagString(T->getTag());

                named_struct->typeMD = CT;

                switch (CT->getTag())
                {
                case dwarf::DW_TAG_structure_type:
                {
                    if (CT->getElements().size() != named_struct->fields.size())
                    {
                        errs() << "Error: error struct field count: " << named_struct->typeName << "\n";
                    }
                    int idx = 0;
                    for (auto *field : CT->getElements())
                    {
                        if (auto *DerivedT = dyn_cast<DIDerivedType>(field))
                        {
                            NamedField *named_field = *(named_struct->fields.begin() + idx);
                            named_field->fieldName = DerivedT->getName().str();
                            named_field->typeMD = DerivedT;
                            // errs()
                            //     << "    ";
                            // errs() << "Name: " << DerivedT->getName() << "    "
                            //        << "Type: " << GetBasicDIType(DerivedT)->getName()
                            //        << "\n";
                        }
                        idx++;
                    }
                    break;
                }
                case dwarf::DW_TAG_class_type:
                {
                    int idx = 0;
                    for (auto *field : CT->getElements())
                    {
                        if (auto *DerivedT = dyn_cast<DIDerivedType>(field))
                        {
                            if (idx >= named_struct->fields.size())
                            {
                                errs() << "ERROR: wrong idx '" << idx << "'\n";
                            }
                            NamedField *named_field = *(named_struct->fields.begin() + idx);
                            named_field->fieldName = DerivedT->getName().str();
                            named_field->typeMD = DerivedT;
                            errs()
                                << "    ";
                            errs() << "Name: " << DerivedT->getName() << "    "
                                   << "Type: " << GetBasicDIType(DerivedT)->getName()
                                   << "\n";
                        }
                        idx++;
                    }
                    break;
                }
                case dwarf::DW_TAG_union_type:
                {
                    break;
                }
                case dwarf::DW_TAG_enumeration_type:
                {
                    break;
                }
                default:
                    break;
                }
                break;
            }
            }
        }
    }
}

char VarAnalysis::ID = 0;

// Register for opt
static RegisterPass<VarAnalysis> X("VarAnalysis", "Var analysis pass");

// Register for clang
static RegisterStandardPasses Y(PassManagerBuilder::EP_EarlyAsPossible,
                                [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM)
                                {
                                    PM.add(new VarAnalysis());
                                });