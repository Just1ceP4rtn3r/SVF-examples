#include "../include/PTA.h"

namespace mqttactic
{
    void PTA::TraverseOnVFG(llvm::Value *key_var, std::set<SemanticKBB *> &SKBBS)
    {
        std::set<const llvm::BasicBlock *> KBBS;
        for (auto sbb : SKBBS)
        {
            KBBS.insert(KBBS.end(), sbb->bb);
        }

        SVFIR *pag = this->Ander->getPAG();
        FIFOWorkList<const VFGNode *> worklist;
        Set<const VFGNode *> use_set;
        Set<const Value *> pts_set;

        PAGNode *pNode = pag->getGNode(pag->getValueNode(key_var));
        if (pNode->hasValue() && pNode->getValue() == key_var && this->Svfg->hasDefSVFGNode(pNode))
        {
            const VFGNode *vNode = this->Svfg->getDefSVFGNode(pNode);
            if (vNode->getValue() != nullptr)
            {
                worklist.push(vNode);
                use_set.insert(vNode);
                pts_set.insert(vNode->getValue());
                while (!worklist.empty())
                {
                    const VFGNode *vNode = worklist.pop();
                    if (vNode->getValue() != nullptr)
                    {
                        dbgs() << "Value: " << *(vNode->getValue()) << "\n";
                        for (auto node_id : vNode->getDefSVFVars())
                        {
                            PAGNode *pag_node = pag->getGNode(node_id);
                            for (auto edge : pag_node->getOutEdges())
                            {
                                if (edge->getEdgeKind() == SVFStmt::Addr)
                                    pag_node = edge->getDstNode();
                            }
                            // dbgs() << pag_node->getId() << "\n";
                            if (this->Svfg->hasDefSVFGNode(pag_node))
                            {
                                const VFGNode *succNode = this->Svfg->getDefSVFGNode(pag_node);
                                // dbgs() << *(succNode->getValue()) << "\n";
                                if (succNode->getValue() && use_set.find(succNode) == use_set.end())
                                {
                                    use_set.insert(succNode);
                                    worklist.push(succNode);
                                    pts_set.insert(succNode->getValue());
                                }
                            }
                        }
                        // use_set.insert(vNode);
                        for (VFGNode::const_iterator it = vNode->OutEdgeBegin(), eit =
                                                                                     vNode->OutEdgeEnd();
                             it != eit; ++it)
                        {
                            VFGEdge *edge = *it;
                            VFGNode *succNode = edge->getDstNode();
                            if (succNode->getValue() && use_set.find(succNode) == use_set.end())
                            {
                                use_set.insert(succNode);
                                worklist.push(succNode);
                                pts_set.insert(succNode->getValue());
                            }
                        }
                    }
                }
                for (Set<const VFGNode *>::iterator vit = use_set.begin(); vit != use_set.end(); vit++)
                {
                    int op_type = 0;
                    std::string Str;
                    raw_string_ostream OS(Str);
                    (*vit)->getValue()->printAsOperand(OS, false);

                    if (const IntraICFGNode *inst = dyn_cast<IntraICFGNode>((*vit)->getICFGNode()))
                    {
                        const Instruction *I = inst->getInst();
                        op_type = IdentifyOperationType(I, (*vit)->getValue(), pts_set);

                        // dbgs()
                        //     << "Value: " << OS.str() << "      " << *I << "\n";
                    }
                    else if (const CallICFGNode *call_inst = dyn_cast<CallICFGNode>((*vit)->getICFGNode()))
                    {
                        const Instruction *I = call_inst->getCallSite();
                        op_type = IdentifyOperationType(I, (*vit)->getValue(), pts_set);
                        // dbgs() << "Value: " << OS.str() << "      " << *I << "\n";
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
                        SemanticKBB *sbb = new SemanticKBB();
                        sbb->bb = (*vit)->getICFGNode()->getBB();
                        sbb->values.push_back((*vit)->getValue());
                        sbb->semantics = op_type;

                        KBBS.insert(KBBS.end(), (*vit)->getICFGNode()->getBB());
                        SKBBS.insert(SKBBS.end(), sbb);
                    }
                    else
                    {
                        for (auto sbb : SKBBS)
                        {
                            if (sbb->bb == (*vit)->getICFGNode()->getBB())
                            {
                                sbb->values.push_back((*vit)->getValue());
                                sbb->semantics |= op_type;
                                break;
                            }
                        }
                    }
                }
                worklist.clear();
                use_set.clear();
            }
        }
    }

    int PTA::IdentifyOperationType(const Instruction *I, const Value *V, Set<const Value *> &pts_set)
    {
        // Normal store/load
        unsigned int opcode = I->getOpcode();
        switch (opcode)
        {
        case Instruction::Call:
        {
            const CallInst *call = static_cast<const CallInst *>(I);
            std::string calledFuncName = "";
            if (call->isIndirectCall() || !(call->getCalledFunction()))
            {
                const GlobalAlias *GV = dyn_cast<GlobalAlias>(call->getCalledOperand());
                if (GV && GV->getAliasee() && GV->getAliasee()->hasName())
                    calledFuncName = GV->getAliasee()->getName().str();
                else
                    break;
            }
            else
            {
                calledFuncName = call->getCalledFunction()->getName().str();
            }

            if (call->getArgOperand(0) == V && calledFuncName != "")
            {
                return IdentifyCallFuncOperation(calledFuncName);
            }

            break;
        }
        case Instruction::Invoke:
        {
            const InvokeInst *call = static_cast<const InvokeInst *>(I);
            std::string calledFuncName = "";
            if (call->isIndirectCall() || !(call->getCalledFunction()))
            {
                const GlobalAlias *GV = dyn_cast<GlobalAlias>(call->getCalledOperand());
                if (GV && GV->getAliasee() && GV->getAliasee()->hasName())
                    calledFuncName = GV->getAliasee()->getName().str();
                else
                    break;
            }
            else
            {
                calledFuncName = call->getCalledFunction()->getName().str();
            }
            if (call->getArgOperand(0) == V && calledFuncName != "")
            {
                return IdentifyCallFuncOperation(calledFuncName);
            }
            break;
        }
        case Instruction::Store:
        {
            const StoreInst *store = static_cast<const StoreInst *>(I);
            // If the value is the rvalue of the `store` instruction
            Value *RightV = store->getOperand(1);
            Value *leftV = store->getOperand(0);
            if (pts_set.find(RightV) != pts_set.end())
            {
                // Link w- operation
                if (pts_set.find(leftV) != pts_set.end())
                {
                    return KeyOperation::WRITE0;
                }
                else
                    return KeyOperation::WRITE1;
            }
            break;
        }
        default:
            break;
        }

        return KeyOperation::READ;
    }

    // Identify operation type of STL function. e.g., vector::push_back
    int PTA::IdentifyCallFuncOperation(std::string func_name)
    {
        std::string OperationFuncRead[] = {"back", "front", "find", "top", "contain"};
        std::string OperationFuncWrite0[] = {"pop_back", "erase", "pop", "delete", "Remove"};
        std::string OperationFuncWrite1[] = {"push_back", "insert", "push", "PushBack", "PushFront"};
        // for (auto op : OperationFuncRead)
        // {
        //     if (func_name.find(op) != std::string::npos)
        //     {
        //         return mqttactic::READ;
        //     }
        // }
        for (auto op : OperationFuncWrite0)
        {
            if (func_name.find(op) != std::string::npos)
            {
                return mqttactic::WRITE0;
            }
        }
        for (auto op : OperationFuncWrite1)
        {
            if (func_name.find(op) != std::string::npos)
            {
                return mqttactic::WRITE1;
            }
        }
        return mqttactic::READ;
    }
}