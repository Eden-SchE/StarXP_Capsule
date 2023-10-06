{
   TGraph *g1, *g2, *g3;
   g1 = new TGraph();
   g2 = new TGraph();
   g3 = new TGraph();

   double N = 6.02E23;
   double H2O_ppm = 0.0001 * 1e-6;
   double Oxygen_ppm = 0.0001 * 1e-6;
   double eV = 1;
   double cm = 10;
   int num_H2O = 0.;
   fstream H2O_absorption;
   H2O_absorption.open("./water_absorption_PHENIX.csv");

   vector<double> H2O_wavelength, H2O_AbsorptionIndex;

   while (!H2O_absorption.eof())
   {
      double buffer1 = 0, buffer2 = 0;
      H2O_absorption >> buffer1 >> buffer2;
      H2O_wavelength.push_back(buffer1);
      H2O_AbsorptionIndex.push_back(buffer2);
      num_H2O++;
   }
   num_H2O = num_H2O - 1;
   double H2O_Ephoton[200];
   double H2O_Absorption[200];
   for (int i = 0; i < num_H2O; i++)
   {
      H2O_Ephoton[i] = 1240 / H2O_wavelength[i] * eV;
      H2O_Absorption[i] = 1 / (H2O_AbsorptionIndex[i] * N * H2O_ppm * 1e-18) * cm; //unit: cm, 1e-6 is from ppm, 1e-18 is from cross section
      //cout << i << " " << H2O_Ephoton[i] << " " << H2O_Absorption[i] << endl;
      g1->SetPoint(i, H2O_Ephoton[i], H2O_Absorption[i]);
   }

   H2O_absorption.close();

   //
   //
   //--------------
   int num_Oxygen = 0;
   fstream Oxygen_absorption;
   Oxygen_absorption.open("./oxygen_absorption_PHENIX.csv", ios::in);

   vector<double> Oxygen_wavelength, Oxygen_AbsorptionIndex;

   while (!Oxygen_absorption.eof())
   {
      double buffer1 = 0, buffer2 = 0;
      Oxygen_absorption >> buffer1 >> buffer2;
      Oxygen_wavelength.push_back(buffer1);
      Oxygen_AbsorptionIndex.push_back(buffer2);
      num_Oxygen++;
   }
   num_Oxygen = num_Oxygen - 1;
   double Oxygen_Ephoton[200];
   double Oxygen_Absorption[200];
   for (int i = 0; i < num_Oxygen; i++)
   {
      Oxygen_Ephoton[i] = 1240 / Oxygen_wavelength[i] * eV;
      Oxygen_Absorption[i] = 1 / (Oxygen_AbsorptionIndex[i] * N * Oxygen_ppm * 1e-18) * cm; //unit: cm, 1e-6 is from ppm, 1e-18 is from cross section
      //cout << i << " " << Oxygen_Ephoton[i] << " " << Oxygen_Absorption[i] << endl;
      g2->SetPoint(i, Oxygen_Ephoton[i], Oxygen_Absorption[i]);
   }

   Oxygen_absorption.close();

   double Total_Ephoton[200], Total_Absorption[200]; // in case num_Oxygen = num_H2O
   for (int i = 0; i < num_Oxygen; i++)
   {
      Total_Ephoton[i] = Oxygen_Ephoton[i];
      //Total_Absorption[i] = Oxygen_Absorption[i] + H2O_Absorption[i];
      Total_Absorption[i] = 1 / ((Oxygen_AbsorptionIndex[i] * Oxygen_ppm + H2O_AbsorptionIndex[i] * H2O_ppm) * N * 1e-18);
      cout  << " " << Total_Ephoton[i] << "*eV " << Total_Absorption[i] << "*cm" << endl;
      g3->SetPoint(i, Total_Ephoton[i], Total_Absorption[i]); 
   }
   g1->SetTitle("H2O absorption length");
   g2->SetTitle("O2 absorption length");
   g3->SetTitle("Total absorption length");
   g1->SetMarkerStyle(8);
   g2->SetMarkerStyle(8);
   g3->SetMarkerStyle(8);
   g1->SetMarkerSize(0.5);
   g2->SetMarkerSize(0.5);
   g3->SetMarkerSize(0.5);
   TCanvas c1;
   c1.Divide(3, 1);
   c1.cd(1);
   g1->Draw();
   c1.cd(2);
   g2->Draw();
   c1.cd(3);
   g3->Draw();
}
