#include "../include/VarAnalysis.h"

namespace mqttactic
{
    std::string VarAnalysis::getBBLabel(const BasicBlock *Node)
    {
        if (!Node->getName().empty())
            return Node->getName().str();
        std::string Str;
        raw_string_ostream OS(Str);
        Node->printAsOperand(OS, false);
        return OS.str();
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

    void VarAnalysis::GetStructDbgInfo(Module &M, DebugInfoFinder *dbgFinder, NamedStructType *named_struct)
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

                // dbgs() << scope_name << "\n";
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
                        int idx = 0;
                        for (auto *field : CT->getElements())
                        {
                            if (auto *DerivedT = dyn_cast<DIDerivedType>(field))
                            {
                                if (DerivedT->getTag() != dwarf::DW_TAG_member)
                                {
                                    continue;
                                }
                                else
                                {
                                    if (DerivedT->getTag() == dwarf::DW_TAG_member && DerivedT->isStaticMember())
                                    {
                                        if (llvm::GlobalVariable *static_var = GetStaticDbgInfo(M, DerivedT))
                                        {
                                            if (!static_var->getName().empty())
                                                VarAnalysis::GlobalVars.insert(std::map<std::string, const llvm::Metadata *>::value_type(static_var->getName().str(), DerivedT));
                                        }
                                        continue;
                                    }
                                }
                                if (idx >= named_struct->fields.size())
                                {
                                    errs() << "ERROR: wrong member " << named_struct->typeName << "\n"
                                           << scope_name << "\n"
                                           << "idx: " << idx << "\n"
                                           << "member size: " << named_struct->fields.size() << "\n";
                                    break;
                                }
                                NamedField *named_field = *(named_struct->fields.begin() + idx);
                                named_field->fieldName = DerivedT->getName().str();
                                named_field->typeMD = DerivedT;
                                // dbgs()
                                //     << "    ";
                                // dbgs() << "Name: " << DerivedT->getName() << "    "
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
                                if (DerivedT->getTag() != dwarf::DW_TAG_member && DerivedT->getTag() != dwarf::DW_TAG_inheritance)
                                {
                                    continue;
                                }
                                else
                                {
                                    if (DerivedT->getTag() == dwarf::DW_TAG_member && DerivedT->isStaticMember())
                                    {
                                        if (llvm::GlobalVariable *static_var = GetStaticDbgInfo(M, DerivedT))
                                        {
                                            if (!static_var->getName().empty())
                                                VarAnalysis::GlobalVars.insert(std::map<std::string, const llvm::Metadata *>::value_type(static_var->getName().str(), DerivedT));
                                        }
                                        continue;
                                    }
                                }
                                if (idx >= named_struct->fields.size())
                                {
                                    errs() << "ERROR: wrong member " << named_struct->typeName << "\n"
                                           << scope_name << "\n"
                                           << "idx: " << idx << "\n"
                                           << "member size: " << named_struct->fields.size() << "\n";
                                    break;
                                }
                                NamedField *named_field = *(named_struct->fields.begin() + idx);
                                named_field->fieldName = DerivedT->getName().str();
                                named_field->typeMD = DerivedT;
                                // dbgs()
                                //     << "    ";
                                // dbgs() << "Name: " << DerivedT->getName() << "    "
                                //        << "Type: " << GetBasicDIType(DerivedT)->getName()
                                //        << "\n";
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

    llvm::GlobalVariable *VarAnalysis::GetStaticDbgInfo(Module &M, DIDerivedType *static_var)
    {

        for (auto &global_var : M.getGlobalList())
        {
            if (!global_var.getName().empty())
            {
                std::string G_name = global_var.getName().str();
                bool flag = false;
                std::string scope = "";
                DIScope *scope_node = static_var->getScope();

                if (!static_var->getName().empty() && G_name.find(static_var->getName().str()) != std::string::npos)
                {
                    flag = true;
                }

                while (scope_node != NULL && flag)
                {
                    scope = scope_node->getName().str();
                    if (G_name.find(scope) == std::string::npos)
                    {
                        flag = false;
                    }
                    scope_node = scope_node->getScope();
                }

                if (flag)
                    return &global_var;
            }
        }
        return nullptr;
    }

    void VarAnalysis::PrintDbgInfo()
    {
        for (auto *named_struct : VarAnalysis::NamedStructTypes)
        {
            dbgs() << named_struct->typeName << "\n{\n";
            for (auto *named_field : named_struct->fields)
            {
                if (named_field->typeMD)
                {
                    std::string Str;
                    raw_string_ostream OS(Str);
                    named_field->type->print(OS, false, true);
                    dbgs() << "    " << named_field->fieldName << " : " << OS.str() << "\n";
                }
            }
            dbgs() << "}\n";
        }

        for (auto git = GlobalVars.begin(); git != GlobalVars.end(); git++)
        {
            std::string Str;
            raw_string_ostream OS(Str);
            git->second->print(OS);
            errs() << git->first << ": " << OS.str() << "\n";
        }
    }

    void VarAnalysis::SearchKeyVar(Module &M, Function &F)
    {
        for (BasicBlock &BB : F)
        {
            for (Instruction &I : BB)
            {
                Instruction *inst = &I;
                std::vector<Value *> inst_value_list;
                unsigned int opcode = inst->getOpcode();
                Use *operand_list = inst->getOperandList();
                inst_value_list.insert(inst_value_list.end(), inst);
                for (int i = 0; i < inst->getNumOperands(); i++)
                {
                    inst_value_list.insert(inst_value_list.end(), operand_list[i]);
                }

                for (Value *operand : inst_value_list)
                {
                    for (KeyVariable *key_var : this->key_variables)
                    {
                        for (auto sbb : SemanticKeyBasicBlocks[key_var])
                        {
                            if (std::find(sbb->values.begin(), sbb->values.end(), operand) != sbb->values.end())
                            {
                                goto RepeatOperandAccess;
                            }
                        }
                        if (ParseVariables(operand, M, F, key_var->name))
                        {
                            // dbgs() << "Instruction: " << I << "\n\n\n\n";
                            PointerAnalyzer->TraverseOnVFG(operand, SemanticKeyBasicBlocks[key_var]);
                            // dbgs() << "----------------------------------\n\n\n\n";
                        }
                    }

                RepeatOperandAccess:
                    continue;
                }
            }
        }
    }

    bool VarAnalysis::ParseVariables(Value *V, Module &M, const Function &F, std::string key_var)
    {
        std::string var_name = "";

        // For Struct type variables:
        // 1. %b11 = getelementptr inbounds %"class.test::Father", %"class.test::Father"* %11, i32 0, i32 1, !dbg !963
        // 2. store i32 2, i32* getelementptr inbounds (%"struct.test::S2", %"struct.test::S2"* @_ZN4test10field_testE, i32 0, i32 1), align 4, !dbg !922
        if (GEPOperator *GEP = dyn_cast<GEPOperator>(V))
        {
            if (GEP->hasAllConstantIndices())
            {
                Type *base = GEP->getSourceElementType();
                int last_idx;
                std::string Str;
                raw_string_ostream OS(Str);
                base->print(OS, false, true);
                var_name += OS.str();
                // dbgs()
                //     << "    " << *V << "\n"
                //     << "    Type: " << OS.str() << "\n"
                //     << "    indices: ";
                for (int i = 1; i != GEP->getNumIndices() + 1; ++i)
                {
                    int idx = cast<ConstantInt>(GEP->getOperand(i))->getZExtValue();
                    last_idx = idx;
                    // dbgs() << idx << ", ";
                }

                if (StructType *base_struct = dyn_cast<StructType>(base))
                {
                    for (auto *named_struct : NamedStructTypes)
                    {
                        if (named_struct->type == base_struct)
                        {
                            int i = 0;
                            for (auto *named_field : named_struct->fields)
                            {
                                if (last_idx == i && named_field->typeMD)
                                {
                                    std::string Str;
                                    raw_string_ostream OS(Str);
                                    named_field->type->print(OS, false, true);
                                    var_name += "::" + named_field->fieldName;
                                    // dbgs() << "    Name: " << named_field->fieldName << " : " << OS.str() << "\n";
                                }
                                i++;
                            }
                        }
                    }
                }
            }
        }

        // For other variables
        else
        {
            if (V->hasName())
            {
                std::map<std::string, const llvm::Metadata *>::iterator git = GlobalVars.find(V->getName().str());
                // static/Global variables
                if (git != GlobalVars.end())
                {
                    std::string n;
                    if (const DIVariable *var = dyn_cast<DIVariable>(git->second))
                    {
                        n = var->getName().str();
                    }
                    else if (const DIDerivedType *var = dyn_cast<DIDerivedType>(git->second))
                    {

                        n = GetScope(var) + var->getName().str();
                    }
                    var_name = n;
                    // dbgs() << "    Global variable Name: " << n << "\n";
                }

                // Local variables
                else
                {
                    var_name = V->getName().str();
                    // dbgs() << "    Local variable Name: " << V->getName().str() << "\n";
                }
            }

            // for (auto &BB : F)
            // {
            //     for (auto &I : BB)
            //     {
            //         const Instruction *inst = &I;
            //         if (const DbgDeclareInst *DbgDeclare = dyn_cast<DbgDeclareInst>(inst))
            //         {
            //             if (DbgDeclare->getAddress() == V)
            //             {
            //                 DILocalVariable *var = DbgDeclare->getVariable();
            //                 if (var)
            //                 {
            //                     errs()
            //                         << "    Local variable Name: " << var->getName().str() << "\n";
            //                 }
            //             }
            //         }
            //         else if (const DbgValueInst *DbgValue = dyn_cast<DbgValueInst>(inst))
            //         {
            //             if (DbgValue->getValue() == V)
            //             {
            //                 DILocalVariable *var = DbgValue->getVariable();
            //                 if (var)
            //                 {
            //                     errs()
            //                         << "    Local variable Name: " << var->getName().str() << "\n";
            //                 }
            //             }
            //         }
            //     }
            // }
        }
        if (var_name != "")
        {
            if (var_name.find(key_var) != std::string::npos && var_name.find(key_var) + key_var.size() == var_name.size())
            {
                // dbgs() << "Found variable Name: " << var_name << "\n";
                return true;
            }
        }
        return false;
    }
}
