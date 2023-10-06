# Star-XP

#### Introduction
Star-XP is a simulation framework constructed specifically for Low Energy X-ray Polarization Detector(LPD). Star-XP is capable
of simulating the emission of particles from given sources and the interaction of these particles with the detector model and the
subsequent electronic readout of LPD.

#### oftware architecture
Base on ROOT v6.22/08, Geant4 v10.3.3 and  YAML 1.2
![Simplified architecture of Star-XP. The dashed boxes represent input or output data products, and the solid boxes represent Star-XP applications.](Information/StarXPFrame.png)

#### Install

```bash
mkdir build
cd build
cmake ..
make
```

#### Instructions


##### Simulation:
1.  To start Simulation：  
    ```bash
    cd build/source/0-Simulation
    ./starXP -[v] [Simucard.card]
    ```
    `-[v]`option to enable the visualization，`-[Simucard.card]`will use the user-specified .card file as the input for simulation.
2.  Simucard file configuration (see example in Simucard.card):
    `Simucard.card` file specifies how the software will simulate signals and in what form.
    ###### GDML configure：  
        DetectorGDML: DetectorConstruction.xml
    ###### Output configure：
        OutputFile: xxx.root
    ###### Simulation method configure：   
        1. To use built-in spectrum simulation, you can configure the following parameters in the Simucard.card file:
        GenTime: This parameter specifies the duration of the simulation in seconds. Set it to a specific value to generate signals for a defined time period. 
        GenEvents: This parameter determines the number of events to generate during the simulation. An event refers to a specific occurrence or signal pattern. Set it to a desired value to generate a specific number of events.
        GenValidEvent:This parameter controls the number of valid events to generate. In the context of GasSD, valid events refer to events that result in energy deposition. Set it to a desired value to generate a specific number of valid events with energy deposition.
        
        2. To use the SimpleGun module for simulation, you can follow these steps:
        GunType:Simple  
        PGName: e+/e-/p+
        PGEnergy:(MeV)
        PGPos:x,y,z
        PGMomDir:px,py,pz
        PGParameters:  there are two ways to set the polarization direction:
            1.Directly setting the polarization direction: You can use the parameters polx, poly, and polz to specify the polarization direction in the x, y, and z axes, respectively. These parameters represent the components of the polarization vector.
            2.Directly setting the polarization direction: You can use the parameters polx, poly, and polz to specify the polarization direction in the x, y, and z axes, respectively. These parameters represent the components of the polarization vector.

        Attention, you can not use GenTime parameter when using SimpleGun module
        
        3. To use .mac file：
        PSSignal:xxx.mac
    ###### Signal selection.：  
        PGType: Signal Name PGParameters: Signal parameters, separated by commas

        GRB: Gamma-Ray Burst, parameters in order are GRB name, zenith angle and azimuth angle, polarization degree

        CXB: Cosmic X-ray Background, parameters in order are lower and upper limits of energy spectrum range

        PCRE: Primary Cosmic Ray Electron Background, parameters in order are lower and upper limits of energy spectrum range, solar modulation parameter (in MV), orbital altitude, and geomagnetic latitude

        PCRP: Primary Cosmic Ray Proton Background, parameters in order are lower and upper limits of energy spectrum range, solar modulation parameter (in MV), orbital altitude, and geomagnetic latitude

        SPE: Secondary Cosmic Ray Electron Background, parameters in order are lower and upper limits of energy spectrum range, and geomagnetic latitude

        SPP: Secondary Cosmic Ray Proton Background, parameters in order are lower and upper limits of energy spectrum range, geomagnetic latitude, and particle direction (facing or away from Earth, 0 for downward, 1 for upward)

        AAP: Atmospheric Albedo Photon Background, parameters in order are lower and upper limits of energy spectrum range, solar modulation parameter (in GV), and geomagnetic latitude

        SAA: South Atlantic Anomaly Proton, parameters in order are lower and upper limits of energy spectrum range


3.  Output.root file organization format (see example in RooScan.C): 
    The files are stored using a custom class called SimEvent. SimEvent serves as a storage unit for an incident event. It utilizes a Hash Table form through TMap to store the trajectories of all particles in the incident event, as well as information about the energy deposition at each step of the trajectory. SimTrack class stores the initial and final state information of a trajectory, while Simdeposit class stores information about a particular step.
    ```bash
    TVector3 GetTruthPosition(): returns the position of the incident particle
    
    TVector3 GetTruthPolarization(): returns the polarization state of the incident particle. The polarization state of a photon is described by the Stokes parameters S1, S2, and S3.
    
    TString GetTruthParticle(): returns the name of the incident particle
    
    TMap* GetTrackMap(): returns the TMap corresponding to the SimTrack class
    
    TMap* GetDepositMap(): returns the TMap corresponding to the SimDeposit class
    
    float GetTimeFlag(): returns the timestamp of the event
    
    SimTrack* GetTrack(int i): returns the SimTrack class corresponding to the i-th trajectory.
    
    Simdeposit* GetDeposit(int i): returns the SimDeposit class corresponding to the i-th step of a trajectory.


SimTrack Member Function:
    int GetPDGID(): returns the PDGID of the particle in the trajectory
    
    int GetTrackID(): returns the track ID of the trajectory
    
    int GetParentID(): returns the parent particle ID
    
    float GetInitMass(): returns the mass of the particle
    
    float GetInitEk(): returns the kinetic energy of the particle
    
    TVector3 GetInitMom(): returns the initial momentum of the particle
    
    TVector3 GetInitPos(): returns the initial position of the particle
    
    double GetInitT(): returns the time when the particle is generated

    TVector3 GetExitMom(): returns the final momentum of the particle
    
    TVector3 GetExitPos(): returns the final position of the particle
    
    double GetExitT(): returns the time when the particle exits
    
    float GetTrackLength(): returns the length of the trajectory
    
    float GetEdep(): returns the energy deposition along the entire trajectory during the simulation
    
    const std::vector<Int_t> GetStepIdx(): returns an array of indices corresponding to the steps in the trajectory


SimDeposit Member Function:

    int GetPDGID(): returns the PDGID of the particle in the trajectory
    
    int GetTrackID(): returns the track ID of the trajectory
    
    int GetParentID(): returns the parent particle ID
    
    int GetCharge(): returns the charge of the particle
    
    TVector3 GetPreMomentum(): returns the momentum before entering the step
    
    TVector3 GetPrePosition(): returns the position after entering the step
    
    double GetPreT(): returns the time before entering the step
    
    TString GetVolumeName(): returns the name of the volume where the step occurs
    
    TVector3 GetPostMomentum(): returns the momentum after leaving the step
    
    TVector3 GetPostPosition(): returns the position after leaving the step
    
    double GetPostT(): returns the time after leaving the step
    
    TString GetProcessName(): returns the name of the physical process that occurs in the step
    
    float GetEdep(): returns the energy deposition in the step
    
    float GetStepLength(): returns the length of the step
    
    bool IsFirstDeposit(): returns whether it is the first energy deposition in the step

    ```

##### Digitization:
1. To start Digitization：  
    ```bash
    cd build/source/1-Digitization
    ./Digitize [Digicard.yml]
```
2.  Filter：  
The Filter section allows you to choose between two options: SaveAllFilter and DiffFilter. If you select SaveAllFilter, all frames will be retained. On the other hand, if you choose DiffFilter, you need to specify the threshold pixel value (ThresholdV) and the threshold pixel count (ThresholdN) for performing interpolation and retaining only pixels that exceed these thresholds.

SaveAllFilter: This filter retains all frames without performing any interpolation.

DiffFilter: This filter performs interpolation and retains only pixels that exceed the specified threshold values. The following parameters need to be specified:

ThresholdV: The threshold pixel value. Pixels exceeding this value will be retained.
ThresholdN: The threshold pixel count. Pixels exceeding this count will be retained.
Based on your specific requirements, you can choose the appropriate filter type and specify the corresponding parameters to process the image frames.

3. Detector
In this section, the user specifies parameters such as the detector's gain collection efficiency, pixel size, and pixel array size. The gas drift velocity and diffusion coefficient are imported as a .yml file. Additionally, the calibration file for detector pixel scaling is imported as a .root file.


#### Contribution Guidelines:

1.Fork this repository.
2.Create a new branch named Feat_xxx (replace xxx with a descriptive name).
3.Commit your changes to the branch.
4.Create a new Pull Request to submit your changes.
5.For development inquiries, please contact liuqian@ucas.ac.cn.
