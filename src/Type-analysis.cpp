//===- wpa.cpp -- Whole program analysis -------------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2017>  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // Whole Program Pointer Analysis
 //
 // Author: Yulei Sui,
 */
#include "SVF-FE/LLVMUtil.h"
#include "Graphs/SVFG.h"
#include "WPA/Andersen.h"
#include "Util/Options.h"
#include "Util/SVFBasicTypes.h"
#include "MemoryModel/PointerAnalysisImpl.h"
#include "SVF-FE/SVFIRBuilder.h"
#include <set>

using namespace llvm;
using namespace std;
using namespace SVF;

void PrintAliasPairs(PointerAnalysis *pta)
{

    SVFIR *pag = pta->getPAG();
    for (SVFIR::iterator lit = pag->begin(), elit = pag->end(); lit != elit; ++lit)
    {
        PAGNode *node1 = lit->second;
        PAGNode *node2 = node1;
        for (SVFIR::iterator rit = lit, erit = pag->end(); rit != erit; ++rit)
        {
            node2 = rit->second;
            if (node1 == node2)
                continue;
            const Function *fun1 = node1->getFunction();
            const Function *fun2 = node2->getFunction();
            SVF::AliasResult result = pta->alias(node1->getId(), node2->getId());
            SVFUtil::outs() << (result == SVF::AliasResult::NoAlias ? "NoAlias" : "MayAlias")
                            << " var" << node1->getId() << "[" << node1->getValueName()
                            << "@" << (fun1 == nullptr ? "" : fun1->getName().str()) << "] --"
                            << " var" << node2->getId() << "[" << node2->getValueName()
                            << "@" << (fun2 == nullptr ? "" : fun2->getName().str()) << "]\n";
        }
    }
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
                // useSet.insert(vNode);

                errs() << "Value: " << *(vNode->getValue()) << "\n"
                       << "Type: "
                       << *(vNode->getValue()->getType()) << "\n";
                for (VFGNode::const_iterator it = vNode->OutEdgeBegin(), eit =
                                                                             vNode->OutEdgeEnd();
                     it != eit; ++it)
                {
                    VFGEdge *edge = *it;
                    VFGNode *succNode = edge->getDstNode();
                    if (succNode->getValue() && useSet.find(succNode) == useSet.end())
                    {
                        useSet.insert(succNode);
                    }
                }

                for (Set<const VFGNode *>::iterator vit = useSet.begin(); vit != useSet.end(); vit++)
                {

                    // const PAGNode *pN = svfg->getLHSTopLevPtr(*vit);
                    // const SVF::Value *val = pN->getValue();
                    errs() << "Value: "
                           << *((*vit)->getValue()) << "\n"
                           << "Type: "
                           << *((*vit)->getValue()->getType()) << "\n";

                    // << "LLVM IR: " << *(*vit) << "\n";
                }
                errs() << "---------------------\n";
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

    Andersen *ander = new Andersen(pag);
    ander->analyze();

    SVFGBuilder svfBuilder;
    SVFG *svfg = svfBuilder.buildFullSVFG(ander);

    // PrintAliasPairs(ander);
    traverseOnVFG(svfg, ander);

    return 0;
}
