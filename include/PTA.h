#ifndef ANDERSEN_PTA
#define ANDERSEN_PTA

#include "SVF-FE/LLVMUtil.h"
#include "Graphs/SVFG.h"
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

namespace mqttactic
{
    // Andersen flow-insensitive pointer analysis
    class PTA
    {
    public:
        SVFModule *SvfModule;
        SVFIR *Pag;
        Andersen *Ander;
        SVFG *Svfg;

        PTA(std::vector<std::string> moduleNameVec)
        {
            this->SvfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
            this->SvfModule->buildSymbolTableInfo();

            // WPAPass *wpa = new WPAPass();
            // wpa->runOnModule(svfModule);

            /// Build Program Assignment Graph (SVFIR)
            SVFIRBuilder builder;
            this->Pag = builder.build(this->SvfModule);

            this->Ander = new Andersen(this->Pag);
            this->Ander->analyze();

            SVFGBuilder svfBuilder;
            this->Svfg = svfBuilder.buildFullSVFG(this->Ander);
        }

        void traverseOnVFG();
    };
}
#endif