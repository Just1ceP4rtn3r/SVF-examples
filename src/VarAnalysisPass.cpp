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

using namespace llvm;

namespace
{
    // class VariableStruct
    // {
    // public:
    // }

    class VarAnalysis : public ModulePass
    {
    public:
        static char ID;
        std::vector<llvm::StructType *> StructSet;

        DIType *GetBasicType(Metadata *MD);
        VarAnalysis() : ModulePass(ID)
        {
        }
        bool runOnModule(Module &M) override
        {
            StructSet = M.getIdentifiedStructTypes();
            for (std::vector<llvm::StructType *>::iterator sit = StructSet.begin(); sit != StructSet.end(); sit++)
            {
                errs() << "Name: " << (*sit)->getName() << "\n"
                       << *(*sit) << "\n";
            }
            errs() << "----------------------------------\n";

            DebugInfoFinder *dbgFinder = new DebugInfoFinder();
            dbgFinder->processModule(M);
            for (const DIType *T : dbgFinder->types())
            {
                if (!T->getName().empty())
                {
                    errs() << "Type:";
                    errs() << ' ' << T->getName() << " ";
                    switch (T->getMetadataID())
                    {
                    case Metadata::DIBasicTypeKind:
                    {
                        auto *BT = dyn_cast<DIBasicType>(T);
                        auto Encoding = dwarf::AttributeEncodingString(BT->getEncoding());
                        if (!Encoding.empty())
                            errs() << Encoding;
                        else
                            errs() << "unknown-encoding(" << BT->getEncoding() << ')';
                        break;
                    }
                    case Metadata::DIDerivedTypeKind:
                    {
                        auto Tag = dwarf::TagString(T->getTag());
                        if (!Tag.empty())
                            errs() << Tag << "\n";
                        else
                            errs() << "unknown-tag(" << T->getTag() << ")\n";
                        break;
                    }
                    case Metadata::DICompositeTypeKind:
                    {
                        auto *CT = dyn_cast<DICompositeType>(T);
                        auto Tag = dwarf::TagString(T->getTag());
                        if (!Tag.empty())
                            errs() << Tag << "\n";
                        else
                            errs() << "unknown-tag(" << CT->getTag() << ")\n";
                        switch (CT->getTag())
                        {
                        case dwarf::DW_TAG_structure_type:
                        {
                            errs() << "{\n";
                            for (auto *field : CT->getElements())
                            {
                                if (auto *DerivedT = dyn_cast<DIDerivedType>(field))
                                {
                                    errs() << "    ";
                                    errs() << "Name: " << DerivedT->getName() << "    "
                                           << "Type: " << GetBasicType(DerivedT)->getName()
                                           << "\n";
                                }
                            }
                            errs() << "}\n";
                            break;
                        }
                        case dwarf::DW_TAG_class_type:
                        {
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
                    errs() << '\n';
                }
            }

            return false;
        }
    };
}

DIType *VarAnalysis::GetBasicType(Metadata *MD)
{
    DIType *ret = nullptr;
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

char VarAnalysis::ID = 0;

// Register for opt
static RegisterPass<VarAnalysis> X("VarAnalysis", "Var analysis pass");

// Register for clang
static RegisterStandardPasses Y(PassManagerBuilder::EP_EarlyAsPossible,
                                [](const PassManagerBuilder &Builder, legacy::PassManagerBase &PM)
                                {
                                    PM.add(new VarAnalysis());
                                });