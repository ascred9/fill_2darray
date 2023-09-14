#include <iostream>
#include <chrono>
#include <thread>

#include "TH1D.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "ThreadPool.h"

using namespace std;

int main()
{  
  int num_threads = 2;
  int number_of_test = 1000;

  TH1F* histStd = new TH1F("histStd", Form("std vs pool%d;times, #muSec;entries", num_threads), 500, 0, 500);
  histStd->SetLineColor(kRed);
  TH1F* histStdSt = new TH1F("histStdSt", Form("std vs pool%d;times, #muSec;entries", num_threads), 500, 0, 500);
  histStdSt->SetLineColor(kBlack);
  TH1F* histPool = new TH1F("histPool", Form("pool%d;times, #muSec;entries", num_threads), 500, 0, 500);

  const unsigned int size1 = 180;
  const unsigned int size2 = 1000;
  unsigned long long** array2D = new unsigned long long *[size1];
  for (unsigned int i = 0; i < size1; ++i)
    array2D[i] = new unsigned long long [size2];

  volatile unsigned long long array2D_st[size1][size2];
  //unsigned long long array2D_st[size1][size2];

  for (int itest = 0; itest < number_of_test; ++itest)
  {
    auto startStd = chrono::high_resolution_clock::now();
    for( unsigned int itheta=0; itheta<size1; itheta++){
      for( unsigned int irho=0; irho<size2; irho++ ){
        array2D[itheta][irho] = 0;
      }
    }
    auto stopStd = std::chrono::high_resolution_clock::now();
    auto durationStd = chrono::duration_cast<chrono::microseconds>(stopStd - startStd);
    //cout << "Std fill matrix duration: " << durationStd.count() << " microSeconds" << endl;
    histStd->Fill(durationStd.count());

    auto startStdS = chrono::high_resolution_clock::now();
    for( unsigned int itheta=0; itheta<size1; itheta++){
      for( unsigned int irho=0; irho<size2; irho++ ){
        array2D_st[itheta][irho] = 0;
      }
    }
    auto stopStdS = std::chrono::high_resolution_clock::now();
    auto durationStdS = chrono::duration_cast<chrono::microseconds>(stopStdS - startStdS);
    //cout << "Std fill matrix duration: " << durationStd.count() << " microSeconds" << endl;
    histStdSt->Fill(durationStdS.count());

    // Use ThreadPool
    auto startPool = chrono::high_resolution_clock::now();
    ThreadPool pool;
    pool.Start(num_threads);
    auto startTask = chrono::high_resolution_clock::now();
    for (int itask=0; itask<num_threads; ++itask){
      auto f_threadp = ([&, itask](){
        int s = (itask+1) * size1 / num_threads;
        for(unsigned int itheta = 0 + itask * size1 / num_threads; itheta < s; ++itheta)
        {
          for(unsigned int irho=0; irho<size2; ++irho)
            array2D[itheta][irho] = 0;
        }
      });
      pool.QueueJob(f_threadp);
    }
    pool.Wait();

    auto stopPool = chrono::high_resolution_clock::now();
    auto durationPool = chrono::duration_cast<chrono::microseconds>(stopPool - startPool);
    auto durationTask = chrono::duration_cast<chrono::microseconds>(stopPool - startTask);
    //cout << "Pool fill matrix duration: " << durationPool.count() << " microSeconds, only task processing:" << durationTask.count() << " microSeconds" << endl;
    histPool->Fill(durationPool.count());
    pool.Stop();
  }

  TCanvas *c = new TCanvas("c", "Comparison", 900, 900);
  histStd->Draw();
  histStdSt->Draw("same");
  histPool->Draw("same");
  TLegend *legend = new TLegend(0.7, 0.6, 0.95, 0.7);
  legend->AddEntry("histStd", "fill directly heap");
  legend->AddEntry("histStdS", "fill directly stack");
  legend->AddEntry("histPool", "fiil by pool");
  legend->Draw();
  c->SaveAs("comparison.png");

  delete[] array2D;
  return 0;
}
