# Star-XP Simulation Framwork

## 介绍
Star-XP 下的 Simulation 软件框架

## 软件架构

Base on ROOT v6.22/08 and Geant4 v10.3.3

## 使用说明


### Simulation:
1.  如何开始模拟：  
    ```bash
    cd build/source/0-Simulation
    ./starXP -[v] [Simucard.card]
    ```
    `-[v]`选择将开启可视化界面，`-[Simucard.card]`将使用用户指定的.card文件作为模拟的输入。
2.  Simucard文件设置(示例请见Simucard.card):  
    `Simucard.card`文件指定软件将以何种形式模拟何种信号

    #### GDML文件设置：  
        DetectorGDML: DetectorConstruction.xml

    #### 输出文件设置：
        OutputFile: xxx.root

    #### 模拟方式设置：   
        1. 使用内置能谱模拟：
        GenTime: 产生指定时长（单位s）  
        GenEvents: 产生指定事例数  
        GenValidEvent:产生指定有效事例数（在GasSD中有能量沉积）
        
        2. 使用SimpleGun模拟：
        GunType:Simple  
        PGName: e+/e-/p+
        PGEnergy:(MeV)
        PGPos:x,y,z
        PGPol:s1,s2,s3
        PGMomDir:px,py,pz
        Simple模式下无法使用GenTime产生模式
        
        3. 使用mac文件模拟：
        PSSignal:xxx.mac

    #### 模拟信号选择：  
        PGType:信号名称
        PGParameters:信号参数，以`,`隔开  
        
        GRB 伽马暴，参数依次为GRB名称，GRB的天顶角和方位角

        CXB 宇宙弥散X射线本底，参数依次为能谱区间的下限,上限

        PCRE 原初宇宙线电子本底，参数依次为能谱区间的下限，上限，太阳调制参数(单位MV)、在轨高度、地磁纬度

        PCRP 原初宇宙线质子本底，参数依次为能谱区间的下限，上限，太阳调制参数(MV),在轨高度、地磁纬度

        SPE 次级宇宙线电子本底，参数依次为能谱区间的下限，上限，地磁纬度

        SPP 次级宇宙线质子本底，参数依次为能谱区间的下限，上限，地磁纬度，粒子来向（面向或背向地球，0为向下，1为向上）

        AAP 大气反照光子本底，参数依次为能谱区间的下限，上限，太阳调制参数（GV），地磁纬度

        SAA 南大西洋异常区质子，参数依次为能谱区间的下限，上限


3.  输出.root文件组织格式 (示例请见RooScan.C)  
    文件以自定义类SimEvent进行存储，SimuEvent以一个入射事例为存储单位，通过TMap的Hash Table形式存储了入射事例中所有粒子的径迹以及每一步径迹的能量沉积等信息。类SimTrack储存一条径迹初态和末态信息，类Simdeposit储存某一Step中的信息。
    ```bash
    SimEvent Member Function:
        float GetTruthEnergy():返回入射粒子的能量
        
        TVector3 GetTruthPosition():返回入射粒子
        
        TVector3 GetTruthPolarization():返回入射粒子的极化状态，光子的极化状态由stokes参量的S1,S2,S3描述
        
        TString GetTruthParticle():返回入射粒子名称
        
        TMap* GetTrackMap():返回SimTrack类对应的TMap
        
        TMap* GetDepositMap():返回SimDeposit类对应的TMap
        
        float GetTimeFlag():返回Event对应的时间戳，仅对GenTime模式可用
        
        SimTrack* GetTrack(int i):返回第i条径迹对应的SimTrack类，注意序号从i=1开始为有效径迹！
        
        Simdeposit* GetDeposit(int i):返回标号i对应的某一径迹某一步对应的SimDeposit类，如何获取某一步对应的标号i请见SimTrack::GetStepIdx()

    
    SimTrack Member Function:
        int GetPDGID():返回径迹粒子的PDGID
        
        int GetTrackID():返回径迹编号
        
        int GetParentID():返回母粒子ID
        
        float GetInitMass():返回粒子质量
        
        float GetInitEk():返回粒子动能
        
        TVector3 GetInitMom():返回粒子初始动量
        
        TVector3 GetInitPos():返回粒子初始位置
        
        double GetInitT():返回粒子产生时间

        TVector3 GetExitMom():返回末态动量
        
        TVector3 GetExitPos():返回粒子末态位置
        
        double GetExitT():返回粒子末态时间
        
        float GetTrackLength():返回径迹长度
        
        float GetEdep():返回整条径迹在模拟过程中的能量沉积
        
        const std::vector<Int_t> GetStepIdx():返回径迹Steps对应的编号数组
    

    SimDeposit Member Function:
    
        int GetPDGID():返回径迹粒子的PDGID
        
        int GetTrackID():返回径迹编号
        
        int GetParentID():返回母粒子ID
        
        int GetCharge():返回电荷数
        
        TVector3 GetPreMomentum():返回进入Step前的动量
        
        TVector3 GetPrePosition():返回进入Step后的坐标
        
        double GetPreT():返回进入Step前的时间
        
        TString GetVolumeName():返回Step所处的Volume
        
        TVector3 GetPostMomentum():返回离开Step后的动量
        
        TVector3 GetPostPosition():返回离开Step后的位置
        
        double GetPostT():返回离开Step后的时间
        
        TString GetProcessName():返回Step中所发生的物理过程
        
        float GetEdep():返回Step中的能量沉积
        
        float GetStepLength():返回Step的径迹长度
        
        bool IsFirstDeposit():是否为首次能量沉积

    ```



