#include "../include/PTA.h"

// void PrintAliasPairs(PointerAnalysis *this->Ander)
// {

//     SVFIR *pag = this->Ander->getPAG();
//     for (SVFIR::iterator lit = pag->begin(), elit = pag->end(); lit != elit; ++lit)
//     {
//         PAGNode *node1 = lit->second;
//         PAGNode *node2 = node1;
//         for (SVFIR::iterator rit = lit, erit = pag->end(); rit != erit; ++rit)
//         {
//             node2 = rit->second;
//             if (node1 == node2)
//                 continue;
//             const Function *fun1 = node1->getFunction();
//             const Function *fun2 = node2->getFunction();
//             SVF::AliasResult result = this->Ander->alias(node1->getId(), node2->getId());
//             SVFUtil::outs() << (result == SVF::AliasResult::NoAlias ? "NoAlias" : "MayAlias")
//                             << " var" << node1->getId() << "[" << node1->getValueName()
//                             << "@" << (fun1 == nullptr ? "" : fun1->getName().str()) << "] --"
//                             << " var" << node2->getId() << "[" << node2->getValueName()
//                             << "@" << (fun2 == nullptr ? "" : fun2->getName().str()) << "]\n";
//         }
//     }
// }

void mqttactic::PTA::traverseOnVFG(llvm::Value *key_var)
{
    SVFIR *pag = this->Ander->getPAG();

    FIFOWorkList<const VFGNode *> worklist;
    Set<const VFGNode *> useSet;

    for (SVFIR::iterator lit = pag->begin(), elit = pag->end(); lit != elit; ++lit)
    {
        PAGNode *pNode = lit->second;

        outs() << "Address: " << pNode->getValue() << "\n";

        if (this->Svfg->hasDefSVFGNode(pNode))
        {
            const VFGNode *vNode = this->Svfg->getDefSVFGNode(pNode);
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
                    // const PAGNode *pN = this->Svfg->getLHSTopLevPtr(*vit);
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