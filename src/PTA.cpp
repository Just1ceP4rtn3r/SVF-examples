#include "../include/PTA.h"

namespace mqttactic
{
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

    std::set<const llvm::BasicBlock *> PTA::TraverseOnVFG(llvm::Value *key_var)
    {
        std::set<const llvm::BasicBlock *> KBBS;

        SVFIR *pag = this->Ander->getPAG();

        FIFOWorkList<const VFGNode *>
            worklist;
        Set<const VFGNode *> useSet;

        for (SVFIR::iterator lit = pag->begin(), elit = pag->end(); lit != elit; ++lit)
        {
            PAGNode *pNode = lit->second;
            if (pNode->hasValue() && pNode->getValue() == key_var && this->Svfg->hasDefSVFGNode(pNode))
            {
                const VFGNode *vNode = this->Svfg->getDefSVFGNode(pNode);
                if (vNode->getValue() != nullptr)
                {

                    worklist.push(vNode);

                    // outs() << "Value: " << *(vNode->getValue()) << "\n"
                    //        << "Type: "
                    //        << *(vNode->getValue()->getType()) << "\n";

                    std::string Str;
                    raw_string_ostream OS(Str);
                    vNode->getValue()->printAsOperand(OS, false);
                    // llvm::errs()
                    //     << "****Pointer Value****\n"
                    //     << OS.str() << "\n"
                    //     << "****KBB****\n"
                    //     << *(vNode->getICFGNode()->getBB()) << "\n";
                    KBBS.insert(KBBS.end(), vNode->getICFGNode()->getBB());
                    // << "VFG: " << *(vNode) << "\n";
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
                        if (const IntraICFGNode *inst = dyn_cast<IntraICFGNode>((*vit)->getICFGNode()))
                        {
                            const Instruction *I = inst->getInst();
                            errs() << *I << "\n";
                        }
                        else if (const CallICFGNode *call_inst = dyn_cast<CallICFGNode>((*vit)->getICFGNode()))
                        {
                            const Instruction *I = call_inst->getCallSite();
                            errs() << *I << "\n";
                        }
                        // const PAGNode *pN = this->Svfg->getLHSTopLevPtr(*vit);
                        // const SVF::Value *val = pN->getValue();
                        // errs() << "Value: "
                        //        << *((*vit)->getValue()) << "\n"
                        //        << "Type: "
                        //        << *((*vit)->getValue()->getType()) << "\n";
                        // llvm::errs()
                        //     << "****Pointer Value****\n"
                        //     << OS.str() << "\n"
                        //     << "****KBB****\n"
                        //     << *(vNode->getICFGNode()->getBB()) << "\n";
                        if (KBBS.find((*vit)->getICFGNode()->getBB()) == KBBS.end())
                        {
                            KBBS.insert(KBBS.end(), (*vit)->getICFGNode()->getBB());
                        }
                    }
                    worklist.clear();
                    useSet.clear();
                }
            }
        }

        return KBBS;
    }

    // int PTA::IdentifyOperationType(const Instruction *I, const Value *V)
    // {
    //     // Normal store/load
    //     unsigned int opcode = I->getOpcode();
    //     switch (opcode)
    //     {
    //     case Instruction::Call:
    //     {
    //         CallInst *call = static_cast<CallInst *>(I);
    //         std::string calledFuncName = "";
    //         if (call->isIndirectCall() || !(call->getCalledFunction()))
    //         {
    //             const GlobalAlias *GV = dyn_cast<GlobalAlias>(call->getCalledOperand());
    //             if (GV && GV->getAliasee() && GV->getAliasee()->hasName())
    //                 calledFuncName = GV->getAliasee()->getName().str();
    //             else
    //                 break;
    //         }
    //         else
    //         {
    //             calledFuncName = call->getCalledFunction()->getName().str();
    //         }
    //         if (!found_end && keyFuncs.find(calledFuncName) != keyFuncs.end() && keyBBs.find(&BB) != keyBBs.end())
    //         {
    //             found_end = traverseFuncToEnd(M, *(call->getCalledFunction()), end, found_end, path, endPath, results);
    //             if (found_end)
    //             {
    //                 path.clear();
    //                 for (std::vector<keyBBPath>::iterator it = results[results.end() - results.begin() - 1].begin(); it != results[results.end() - results.begin() - 1].end(); it++)
    //                 {
    //                     path.push_back((*it));
    //                 }
    //                 results.pop_back();
    //             }
    //         }
    //         break;
    //     }
    //     case Instruction::Invoke:
    //     {
    //         InvokeInst *call = static_cast<InvokeInst *>(I);
    //         std::string calledFuncName = "";
    //         if (call->isIndirectCall() || !(call->getCalledFunction()))
    //         {
    //             const GlobalAlias *GV = dyn_cast<GlobalAlias>(call->getCalledOperand());
    //             if (GV && GV->getAliasee() && GV->getAliasee()->hasName())
    //                 calledFuncName = GV->getAliasee()->getName().str();
    //             else
    //                 break;
    //         }
    //         else
    //         {
    //             calledFuncName = call->getCalledFunction()->getName().str();
    //         }
    //         if (!found_end && keyFuncs.find(calledFuncName) != keyFuncs.end() && keyBBs.find(&BB) != keyBBs.end())
    //         {
    //             found_end = traverseFuncToEnd(M, *(call->getCalledFunction()), end, found_end, path, endPath, results);
    //             if (found_end)
    //             {
    //                 path.clear();
    //                 for (std::vector<keyBBPath>::iterator it = results[results.end() - results.begin() - 1].begin(); it != results[results.end() - results.begin() - 1].end(); it++)
    //                 {
    //                     path.push_back((*it));
    //                 }
    //                 results.pop_back();
    //             }
    //         }
    //         break;
    //     }
    //     case Instruction::Store:
    //     {
    //         StoreInst *store = static_cast<StoreInst *>(I);
    //         // If the value is the rvalue of the `store` instruction
    //         if (V == store->getOperand(1))
    //         {
    //             return KeyOperation::WRITE1;
    //         }
    //     }
    //     default:
    //         break;
    //     }
    // }
}