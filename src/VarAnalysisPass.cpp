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
    class VarAnalysis : public ModulePass
    {
    public:
        static char ID;
        std::vector<llvm::StructType *> StructSet;

        void traverseMetadata(MDNode *MD);
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

            DebugInfoFinder *dbgFinder = new DebugInfoFinder();
            dbgFinder->processModule(M);
            for (const DIType *T : dbgFinder.types())
            {
                errs() << "Type:";
                if (!T->getName().empty())
                    errs() << ' ' << T->getName();
                printFile(errs(), T->getFilename(), T->getDirectory(), T->getLine());
                if (auto *BT = dyn_cast<DIBasicType>(T))
                {
                    errs() << " ";
                    auto Encoding = dwarf::AttributeEncodingString(BT->getEncoding());
                    if (!Encoding.empty())
                        errs() << Encoding;
                    else
                        errs() << "unknown-encoding(" << BT->getEncoding() << ')';
                }
                else
                {
                    errs() << ' ';
                    auto Tag = dwarf::TagString(T->getTag());
                    if (!Tag.empty())
                        errs() << Tag;
                    else
                        errs() << "unknown-tag(" << T->getTag() << ")";
                }
                if (auto *CT = dyn_cast<DICompositeType>(T))
                {
                    if (auto *S = CT->getRawIdentifier())
                        errs() << " (identifier: '" << S->getString() << "')";
                }
                errs() << '\n';
            }

            // for (llvm::Module::named_metadata_iterator nmdit = M.named_metadata_begin(); nmdit != M.named_metadata_end(); nmdit++)
            // {
            //     for (llvm::NamedMDNode::op_iterator mdit = (*nmdit).op_begin(); mdit != (*nmdit).op_end(); mdit++)
            //     {
            //         traverseMetadata((*mdit));
            //     }
            // }

            return false;
        }

        void traverseMetadata(MDNode *MD)
        {
            // if (DICompositeType *comp_node = dynamic_cast<DICompositeType *>(MD))
            // {
            //     comp_node->getTag()
            // }
            // else
            // {
            // }

            switch (MD->getMetadataID())
            {
            case GenericDINodeKind:
            case DISubrangeKind:
            case DIEnumeratorKind:
            case DIBasicTypeKind:
            case DIDerivedTypeKind:
            case DICompositeTypeKind:
            {
                DICompositeType *comp_node = dynamic_cast<DICompositeType *>(MD);
                switch (comp_node->getTag())
                {
                case dwarf::DW_TAG_structure_type:
                {
                    errs() << "DW_TAG_structure_type: " << *comp_node << '\n';
                    break;
                }
                case dwarf::DW_TAG_class_type:
                {
                    errs() << "DW_TAG_class_type";
                    break;
                }
                case dwarf::DW_TAG_union_type:
                {
                    errs() << "DW_TAG_class_type";
                    break;
                }
                case dwarf::DW_TAG_enumeration_type:
                {
                    errs() << "DW_TAG_class_type";
                    break;
                }
                default:
                    break;
                }
                break;
            }
            case DISubroutineTypeKind:
            case DIFileKind:
            case DICompileUnitKind:
            case DISubprogramKind:
            case DILexicalBlockKind:
            case DILexicalBlockFileKind:
            case DINamespaceKind:
            case DICommonBlockKind:
            case DITemplateTypeParameterKind:
            case DITemplateValueParameterKind:
            case DIGlobalVariableKind:
            case DILocalVariableKind:
            case DILabelKind:
            case DIObjCPropertyKind:
            case DIImportedEntityKind:
            case DIModuleKind:
            default:
                break;
            }
        }
    };
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