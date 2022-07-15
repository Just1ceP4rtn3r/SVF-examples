#include "SVF-FE/LLVMUtil.h"
#include "Graphs/SVFG.h"
#include "Graphs/ICFG.h"
#include "WPA/Andersen.h"
#include "Util/Options.h"
#include "Util/SVFBasicTypes.h"
#include "MemoryModel/PointerAnalysisImpl.h"
#include "SVF-FE/SVFIRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include <set>

using namespace llvm;
using namespace std;
using namespace SVF;

void findVariable(PointerAnalysis *pta, std::string str)
{
    ICFG *icfg = pta->getICFG();

    pta->dumpAllPts();

    SVFUtil::outs() << "------------------\n\n\n";

    pta->dumpAllTypes();

    SVFUtil::outs() << *(pta->getPAG()->getGNode(46));

    // FIFOWorkList<const ICFGNode *> worklist;
    // Set<const ICFGNode *> visited;
    // worklist.push(iNode);

    // /// Traverse along VFG
    // while (!worklist.empty())
    // {
    //     const ICFGNode *iNode = worklist.pop();
    //     for (ICFGNode::const_iterator it = iNode->OutEdgeBegin(), eit =
    //                                                                   iNode->OutEdgeEnd();
    //          it != eit; ++it)
    //     {
    //         ICFGEdge *edge = *it;
    //         ICFGNode *succNode = edge->getDstNode();
    //         if (visited.find(succNode) == visited.end())
    //         {
    //             visited.insert(succNode);
    //             worklist.push(succNode);
    //         }
    //     }
    // }
}

void traverseOnVFG(const SVFG *svfg, PointerAnalysis *pta)
{
    SVFIR *pag = pta->getPAG();

    FIFOWorkList<const VFGNode *> worklist;
    Set<const VFGNode *> useSet;

    for (SVFIR::iterator lit = pag->begin(), elit = pag->end(); lit != elit; ++lit)
    {
        PAGNode *pNode = lit->second;
        if (svfg->hasDefSVFGNode(pNode))
        {
            const VFGNode *vNode = svfg->getDefSVFGNode(pNode);
            if (vNode->getValue() != nullptr && vNode->getValue()->getName().str().find("key_var") != std::string::npos)
            {
                worklist.push(vNode);

                // outs() << "Value: " << *(vNode->getValue()) << "\n"
                //        << "Type: "
                //        << *(vNode->getValue()->getType()) << "\n";

                std::string Str;
                raw_string_ostream OS(Str);
                vNode->getValue()->printAsOperand(OS, false);
                SVFUtil::outs()
                    << "Value: " << OS.str() << "\n"
                    << "VFG: " << *(vNode) << "\n";
                while (!worklist.empty())
                {
                    const VFGNode *vNode = worklist.pop();
                    if (vNode->getValue() != nullptr)
                    {
                        // useSet.insert(vNode);
                        for (VFGNode::const_iterator it = vNode->OutEdgeBegin(), eit =
                                                                                     vNode->OutEdgeEnd();
                             it != eit; ++it)
                        {
                            VFGEdge *edge = *it;
                            VFGNode *succNode = edge->getDstNode();
                            if (succNode->getValue() && useSet.find(succNode) == useSet.end())
                            {
                                useSet.insert(succNode);
                                worklist.push(succNode);
                            }
                        }
                    }
                }
                for (Set<const VFGNode *>::iterator vit = useSet.begin(); vit != useSet.end(); vit++)
                {
                    std::string Str;
                    raw_string_ostream OS(Str);
                    (*vit)->getValue()->printAsOperand(OS, false);
                    // const PAGNode *pN = svfg->getLHSTopLevPtr(*vit);
                    // const SVF::Value *val = pN->getValue();
                    // errs() << "Value: "
                    //        << *((*vit)->getValue()) << "\n"
                    //        << "Type: "
                    //        << *((*vit)->getValue()->getType()) << "\n";
                    SVFUtil::outs()
                        << "Value: " << OS.str() << "\n"
                        << "VFG: " << *(*vit) << "\n";
                    //    << "Edge: "
                    //    << edge->getEdgeKind() << "\n";
                    //
                }
                SVFUtil::outs() << "---------------------\n";
                worklist.clear();
                useSet.clear();
            }
        }
    }
}

int main(int argc, char **argv)
{

    int arg_num = 0;
    char **arg_value = new char *[argc];
    std::vector<std::string> moduleNameVec;
    LLVMUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
    cl::ParseCommandLineOptions(arg_num, arg_value,
                                "Type analysis\n");

    // if (Options::WriteAnder == "ir_annotator")
    // {
    //     LLVMModuleSet::getLLVMModuleSet()->preProcessBCs(moduleNameVec);
    // }

    SVFModule *svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
    svfModule->buildSymbolTableInfo();

    // WPAPass *wpa = new WPAPass();
    // wpa->runOnModule(svfModule);

    /// Build Program Assignment Graph (SVFIR)
    SVFIRBuilder builder;
    SVFIR *pag = builder.build(svfModule);
    ICFG *icfg = pag->getICFG();

    Andersen *ander = new Andersen(pag);
    ander->analyze();

    SVFGBuilder svfBuilder;
    SVFG *svfg = svfBuilder.buildFullSVFG(ander);

    findVariable(ander, "123");

    svfg->dump("./svfg");
    pag->dump("./pag");
    icfg->dump("./icfg");

    return 0;
}
