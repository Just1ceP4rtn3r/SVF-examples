#include "../include/PTA.h"

namespace mqttactic
{
    std::set<SemanticKBB *> PTA::TraverseOnVFG(llvm::Value *key_var)
    {
        std::set<const llvm::BasicBlock *> KBBS;
        std::set<SemanticKBB *> SKBBS;

        SVFIR *pag = this->Ander->getPAG();
        FIFOWorkList<const VFGNode *> worklist;
        Set<const VFGNode *> use_set;
        Set<const Value *> pts_set;

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

                            errs()
                                << "Value: " << OS.str() << "      " << *I << "\n";
                        }
                        else if (const CallICFGNode *call_inst = dyn_cast<CallICFGNode>((*vit)->getICFGNode()))
                        {
                            const Instruction *I = call_inst->getCallSite();
                            op_type = IdentifyOperationType(I, (*vit)->getValue(), pts_set);
                            errs() << "Value: " << OS.str() << "      " << *I << "\n";
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
                            std::set<mqttactic::SemanticKBB *>::iterator sbb = SKBBS.begin() + (KBBS.find((*vit)->getICFGNode()->getBB()) - KBBS.begin());
                            (*sbb)->semantics &= op_type;
                        }
                    }
                    worklist.clear();
                    use_set.clear();
                }
            }
        }

        return SKBBS;
    }

    int PTA::IdentifyOperationType(const Instruction *I, const Value *V, Set<Value *> &pts_set)
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
            if (V == store->getOperand(1))
            {

                Value *leftV = store->getOperand(0);
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

    int PTA::IdentifyCallFuncOperation(std::string func_name)
    {
        // for (auto op : mqttactic::OperationFuncRead)
        // {
        //     if (func_name.find(op) != std::string::npos)
        //     {
        //         return mqttactic::READ;
        //     }
        // }
        for (auto op : mqttactic::OperationFuncWrite0)
        {
            if (func_name.find(op) != std::string::npos)
            {
                return mqttactic::WRITE0;
            }
        }
        for (auto op : mqttactic::OperationFuncWrite1)
        {
            if (func_name.find(op) != std::string::npos)
            {
                return mqttactic::WRITE1;
            }
        }
        return mqttactic::READ;
    }
}