//*********************************************
//  This is Geant4 Template
//                                  author:Qian
//

#include "MyPrimaryGeneratorGPS.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"

#include "Verbose.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyPrimaryGeneratorGPS::MyPrimaryGeneratorGPS()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(0)
{
    if (verbose)
        G4cout << "====>MyPrimaryGeneratorGPS::MyPrimaryGeneratorGPS()" << G4endl;

    fParticleGun  = new G4GeneralParticleSource();
    fCosmicRayList = CosmicRayList::GetInstance();
    fCosmicRay = fCosmicRayList->GetGenerator();

    fCosmicRay->SetParticle(fParticleGun->GetParticleDefinition()->GetParticleName());

    //#PartGPS 2. gps相关参数解释
    /*
    1. 查阅： http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/BackupVersions/V10.3/html/ch02s07.html 
    2. mac里的参数大部分与particleGun相似，仅把不同的参数简要说明如下
        /gps/pos/type [dist]    入射粒子用 Point(default), Plane, Beam, Surface, Volume的方式产生
        /gps/pos/shape [shape]  对于Plane类型，可选按 Circle, Annulus(环), Ellipse, Square, Rectangle 产生
                                对于Surface/Volume类型，可选按 Sphere, Ellipsoid, Cylinder, Para(parallelpiped) 产生
        /gps/pos/centre [X Y Z] 设定这个形状的中心点位置
        /gps/pos/rot1   [X Y Z] 设定这个形状旋转矩阵
        /gps/pos/halfx, halfy, halfz, radius, inner_raddius, sigma_r, sigma_x, sigma_y ....

        /gps/ang/type [dist]    入射粒子用 iso(default), cos, planar, beam1d, beam2d, focused, user来产生
        /gps/ang/rot1 [X Y Z]   设定这个形状旋转矩阵
        /gps/ang/mintheta, maxtheta, minphi, maxphi, sigma_r, sigma_x, sigma_y ...

        /gps/ene/type [dist]    入射粒子用 Mono(单能), E-E0 [mono] 
                                         Lin, I0[intercept] + E * m [gradient]
                                         Pow, E^alpha [alpha]
                                         Exp, Exp(-E/E0 [ezero])
                                         Gauss, Gauss(E0, sigma) [sigma]
                                         Brem, Bremsstrahlung(T) [temp]
                                         Bbody, BlackBody(T) [temp]
                                         Cdg, cosmic diffuse gamma ray(Eb, a1, a2)
                                         Arb, (point-wise sepctrum)
                                         User, (User-defined histogram)
        /gps/ene/min, max, mono, sigma, alpha, ...

        /gps/hist/type [type]   用户设定的直方图分布类型，包括 
                                biasx, biasy, biasz
                                biast, biasp (angle theta, angle phi)
                                baispt, biaspp (pisition theta, position phi)
                                biase, 
                                theta, phi, energy, arb, epn
        /gps/hist/file [file]   读入一个文件
        /gps/hist/inter [type]  设定插值的方式，有 Lin, Log, Exp, Spline等不同插值方法

        1）举例，常规操作：
        /gps/particle gamma

        /gps/pos/type Plane         
	    /gps/pos/shape Square
	    /gps/pos/centre 1 2 1 cm
	    /gps/pos/halfx 2 cm
	    /gps/pos/halfy 2 cm     

        /gps/ang/type cos
        /gps/ang/mintheta  91 deg
        /gps/ang/maxtheta 180 de   

        /gps/ene/type Gauss
        /gps/ene/mono 400 MeV
        /gps/ene/sigma 50 MeV

        2）或者用自己的hist来放不同能量分布，比如
        # energy distribution
        /gps/ene/type Arb
        /gps/hist/type arb
        /gps/hist/point 1.0 2.
        /gps/hist/point 2.0 5.
        /gps/hist/point 7.0 1.
        /gps/hist/point 10. 1.
        /gps/hist/inter Spline
 
        3）或者用自己的hist来放各种不同的分布，比如
        #position
        /gps/pos/type Surface
        /gps/pos/shape Sphere
        /gps/pos/radius 5. cm
        #
        # biasing the positional theta - phi generator
        # it is used only in for sphere, cylinder and ellipsoid surface distribution 
        # incident surface is a section of the sphere only
        #
        /gps/hist/type biaspt
        /gps/hist/point 0. 0.
        /gps/hist/point 0.5 0.
        /gps/hist/point 1. 1.
        #
        /gps/hist/type biaspp
        /gps/hist/point 0. 0.
        /gps/hist/point 0.75 0.
        /gps/hist/point 1. 1.

        # angular distribution
        /gps/ang/type iso
        /gps/ang/surfnorm false
        #
        # biasing the angular theta 
        /gps/hist/type biast
        /gps/hist/point 0. 0.
        /gps/hist/point 0.1 1.
        /gps/hist/point 1. 1.
        # biasing the angular phi
        /gps/hist/type biasp
        /gps/hist/point 0. 0.
        /gps/hist/point 0.125 1.
        /gps/hist/point 0.875 4.
        /gps/hist/point 1. 1.

        # energy distribution
        /gps/ene/type Arb
        /gps/ene/diffspec 0
        /gps/hist/type arb
        /gps/hist/point 0.0 11.
        /gps/hist/point 1.0 10.
        /gps/hist/point 11.0 0.
        /gps/hist/inter Lin
    */
}

//MyPrimaryGeneratorGPS::MyPrimaryGeneratorGPS(PGPSConfig ConfigPS)
//    : G4VUserPrimaryGeneratorAction(),
//      fParticleGun(0),
//      fConfigPS(ConfigPS)
//{
//    if (verbose)
//        G4cout << "====>MyPrimaryGeneratorGPS::MyPrimaryGeneratorGPS()" << G4endl;
//
//    fParticleGun  = new G4GeneralParticleSource();
//}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MyPrimaryGeneratorGPS::~MyPrimaryGeneratorGPS()
{
    delete fParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyPrimaryGeneratorGPS::GeneratePrimaries(G4Event *anEvent)
{
    if (verbose)
        G4cout << "====>MyPrimaryGeneratorGPS::GeneratePrimaries()" << G4endl;

    fParticleGun->GeneratePrimaryVertex(anEvent);
    fCosmicRay->SetParticleEnergy(fParticleGun->GetParticleEnergy());
    fCosmicRay->SetParticlePosition(fParticleGun->GetParticlePosition());
    fCosmicRay->SetParticlePolarization(fParticleGun->GetParticlePolarization());
    fCosmicRay->SetParticleMomentumDirection(fParticleGun->GetParticleMomentumDirection());
}