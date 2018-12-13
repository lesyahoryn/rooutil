//  .
// ..: P. Chang, philip@physics.ucsd.edu

#ifndef ttreex_h
#define ttreex_h

// C/C++
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdarg.h>
#include <functional>
#include <cmath>
#include <utility>

// ROOT
#include "TBenchmark.h"
#include "TBits.h"
#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TChainElement.h"
#include "TTreeCache.h"
#include "TTreePerfStats.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TString.h"
#include "TLorentzVector.h"
#include "Math/LorentzVector.h"

#include "printutil.h"

//#define MAP std::unordered_map
//#define STRING std::string
#define MAP std::map
#define STRING TString

///////////////////////////////////////////////////////////////////////////////////////////////
// LorentzVector typedef that we use very often
///////////////////////////////////////////////////////////////////////////////////////////////
typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<float> > LV;

namespace RooUtil
{

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // TTreeX class
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // NOTE: This class assumes accessing TTree in the SNT style which uses the following,
    // https://github.com/cmstas/Software/blob/master/makeCMS3ClassFiles/makeCMS3ClassFiles.C
    // It is assumed that the "template" class passed to this class will have
    // 1. "Init(TTree*)"
    // 2. "GetEntry(uint)"
    // 3. "progress(nevtProc'ed, total)"
    class TTreeX
    {

        public:
        enum kType
        {
            kInt_t      =  1,
            kBool_t     =  2,
            kFloat_t    =  3,
            kTString    =  4,
            kLV         =  5,
            kVecInt_t   = 11,
            kVecBool_t  = 12,
            kVecFloat_t = 13,
            kVecTString = 14,
            kVecLV      = 15
        };
        typedef std::vector<LV>::const_iterator lviter;

        private:
        TTree* ttree;
        std::map<TString, Int_t  > mapInt_t;
        std::map<TString, Bool_t > mapBool_t;
        std::map<TString, Float_t> mapFloat_t;
        std::map<TString, TString> mapTString;
        std::map<TString, LV     > mapLV;
        std::map<TString, TBits  > mapTBits;
        std::map<TString, unsigned long long> mapULL;
        std::map<TString, std::vector<Int_t  > > mapVecInt_t;
        std::map<TString, std::vector<Bool_t > > mapVecBool_t;
        std::map<TString, std::vector<Float_t> > mapVecFloat_t;
        std::map<TString, std::vector<TString> > mapVecTString;
        std::map<TString, std::vector<LV     > > mapVecLV;

        std::map<TString, Bool_t > mapIsBranchSet;

        public:
        TTreeX();
        TTreeX(TString treename, TString title);
        TTreeX(TTree* tree);
        ~TTreeX();
        TTree* getTree() { return ttree; }
        void setTree(TTree* tree) { ttree = tree; }
        void* getValPtr(TString brname);
        template <class T>
        T* get(TString brname, int entry=-1);
        void fill() { ttree->Fill(); }
        void write() { ttree->Write(); }

        template <class T>
        void createBranch(TString);
        template <class T>
        void setBranch(TString, T);
        template <class T>
        const T& getBranch(TString, bool=true);
        template <class T>
        void createBranch(T&);
        template <class T>
        void setBranch(T&);
        template <class T>
        void pushbackToBranch(TString, T);

        void sortVecBranchesByPt(TString, std::vector<TString>, std::vector<TString>, std::vector<TString>);
        template <class T>
        std::vector<T> sortFromRef( std::vector<T> const& in, std::vector<std::pair<size_t, lviter> > const& reference);
        struct ordering
        {
            bool operator ()(std::pair<size_t, lviter> const& a, std::pair<size_t, lviter> const& b) 
            {
                return (*(a.second)).pt() > (*(b.second)).pt();
            }
        };
        void createFlatBranch(std::vector<TString>, std::vector<TString>, std::vector<TString>, std::vector<TString>, int);
        void setFlatBranch(std::vector<TString>, std::vector<TString>, std::vector<TString>, std::vector<TString>, int);

        void clear();
        void save(TFile*);
    };

    //_________________________________________________________________________________________________
    template <> void TTreeX::setBranch<Int_t               >(TString bn, Int_t                val);
    template <> void TTreeX::setBranch<Bool_t              >(TString bn, Bool_t               val);
    template <> void TTreeX::setBranch<Float_t             >(TString bn, Float_t              val);
    template <> void TTreeX::setBranch<TString             >(TString bn, TString              val);
    template <> void TTreeX::setBranch<LV                  >(TString bn, LV                   val);
    template <> void TTreeX::setBranch<TBits               >(TString bn, TBits                val);
    template <> void TTreeX::setBranch<unsigned long long  >(TString bn, unsigned long long   val);
    template <> void TTreeX::setBranch<std::vector<Int_t  >>(TString bn, std::vector<Int_t  > val);
    template <> void TTreeX::setBranch<std::vector<Bool_t >>(TString bn, std::vector<Bool_t > val);
    template <> void TTreeX::setBranch<std::vector<Float_t>>(TString bn, std::vector<Float_t> val);
    template <> void TTreeX::setBranch<std::vector<TString>>(TString bn, std::vector<TString> val);
    template <> void TTreeX::setBranch<std::vector<LV     >>(TString bn, std::vector<LV     > val);
    template <> void TTreeX::pushbackToBranch<Int_t        >(TString bn, Int_t       val);
    template <> void TTreeX::pushbackToBranch<Bool_t       >(TString bn, Bool_t      val);
    template <> void TTreeX::pushbackToBranch<Float_t      >(TString bn, Float_t     val);
    template <> void TTreeX::pushbackToBranch<TString      >(TString bn, TString     val);
    template <> void TTreeX::pushbackToBranch<LV           >(TString bn, LV          val);

    //_________________________________________________________________________________________________
    template <> const Int_t               & TTreeX::getBranch<Int_t               >(TString bn, bool check);
    template <> const Bool_t              & TTreeX::getBranch<Bool_t              >(TString bn, bool check);
    template <> const Float_t             & TTreeX::getBranch<Float_t             >(TString bn, bool check);
    template <> const TString             & TTreeX::getBranch<TString             >(TString bn, bool check);
    template <> const LV                  & TTreeX::getBranch<LV                  >(TString bn, bool check);
    template <> const TBits               & TTreeX::getBranch<TBits               >(TString bn, bool check);
    template <> const unsigned long long  & TTreeX::getBranch<unsigned long long  >(TString bn, bool check);
    template <> const std::vector<Int_t  >& TTreeX::getBranch<std::vector<Int_t  >>(TString bn, bool check);
    template <> const std::vector<Bool_t >& TTreeX::getBranch<std::vector<Bool_t >>(TString bn, bool check);
    template <> const std::vector<Float_t>& TTreeX::getBranch<std::vector<Float_t>>(TString bn, bool check);
    template <> const std::vector<TString>& TTreeX::getBranch<std::vector<TString>>(TString bn, bool check);
    template <> const std::vector<LV     >& TTreeX::getBranch<std::vector<LV     >>(TString bn, bool check);

    //_________________________________________________________________________________________________
    template <> void TTreeX::createBranch<Int_t               >(TString bn);
    template <> void TTreeX::createBranch<Bool_t              >(TString bn);
    template <> void TTreeX::createBranch<Float_t             >(TString bn);
    template <> void TTreeX::createBranch<TString             >(TString bn);
    template <> void TTreeX::createBranch<LV                  >(TString bn);
    template <> void TTreeX::createBranch<TBits               >(TString bn);
    template <> void TTreeX::createBranch<unsigned long long  >(TString bn);
    template <> void TTreeX::createBranch<std::vector<Int_t  >>(TString bn);
    template <> void TTreeX::createBranch<std::vector<Bool_t >>(TString bn);
    template <> void TTreeX::createBranch<std::vector<Float_t>>(TString bn);
    template <> void TTreeX::createBranch<std::vector<TString>>(TString bn);
    template <> void TTreeX::createBranch<std::vector<LV     >>(TString bn);

    //_________________________________________________________________________________________________
    template <> void TTreeX::setBranch<std::map<TString, std::vector<Int_t>>>(std::map<TString, std::vector<Int_t>>& objidx);
    template <> void TTreeX::createBranch<std::map<TString, std::vector<Int_t>>>(std::map<TString, std::vector<Int_t>>& objidx);

    template <class T>
    std::vector<T> TTreeX::sortFromRef( std::vector<T> const& in, std::vector<std::pair<size_t, TTreeX::lviter> > const& reference)
    {
        std::vector<T> ret(in.size());

        size_t const size = in.size();
        for (size_t i = 0; i < size; ++i)
            ret[i] = in[reference[i].first];

        return ret;
    }

}

//_________________________________________________________________________________________________
template <class T>
T* RooUtil::TTreeX::get(TString brname, int entry)
{
    if (entry >= 0)
        ttree->GetEntry(entry);
    return (T*) getValPtr(brname);
}

#endif
