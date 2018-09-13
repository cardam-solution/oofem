/*
 *
 *                 #####    #####   ######  ######  ###   ###
 *               ##   ##  ##   ##  ##      ##      ## ### ##
 *              ##   ##  ##   ##  ####    ####    ##  #  ##
 *             ##   ##  ##   ##  ##      ##      ##     ##
 *            ##   ##  ##   ##  ##      ##      ##     ##
 *            #####    #####   ##      ######  ##     ##
 *
 *
 *             OOFEM : Object Oriented Finite Element Code
 *
 *               Copyright (C) 1993 - 2013   Borek Patzak
 *
 *
 *
 *       Czech Technical University, Faculty of Civil Engineering,
 *   Department of Structural Mechanics, 166 29 Prague, Czech Republic
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef hydratingconcretemat_h
#define hydratingconcretemat_h

#include "tm/Materials/isoheatmat.h"
#include "tm/Materials/hydratingisoheatmat.h"

///@name Input fields for HydratingConcreteMat
//@{
#define _IFT_HydratingConcreteMat_Name "hydratingconcretemat"
#define _IFT_HydratingConcreteMat_referenceTemperature "referencetemperature"
#define _IFT_HydratingConcreteMat_castAt "castat"
#define _IFT_HydratingConcreteMat_hydrationModelType "hydrationmodeltype"
#define _IFT_HydratingConcreteMat_maxModelIntegrationTime "maxmodelintegrationtime"
#define _IFT_HydratingConcreteMat_minModelTimeStepIntegrations "minmodeltimestepintegrations"
#define _IFT_HydratingConcreteMat_conductivitytype "conductivitytype"
#define _IFT_HydratingConcreteMat_capacitytype "capacitytype"
#define _IFT_HydratingConcreteMat_densitytype "densitytype"
#define _IFT_HydratingConcreteMat_activationEnergy "activationenergy"
#define _IFT_HydratingConcreteMat_massCement "masscement"
#define _IFT_HydratingConcreteMat_reinforcementDegree "reinforcementdegree"
#define _IFT_HydratingConcreteMat_tau "tau"
#define _IFT_HydratingConcreteMat_beta "beta"
#define _IFT_HydratingConcreteMat_B1 "b1"
#define _IFT_HydratingConcreteMat_B2 "b2"
#define _IFT_HydratingConcreteMat_eta "eta"
#define _IFT_HydratingConcreteMat_DoHInf "dohinf"
#define _IFT_HydratingConcreteMat_DoH1 "doh1"
#define _IFT_HydratingConcreteMat_P1 "p1"
#define _IFT_HydratingConcreteMat_qpot "qpot"
//@}

namespace oofem {
/**
 * This class implements various phenomenological and affinity hydration models. No coupling with relative humidity
 * is considered. Heat capacity and thermal conductivity can be set constant or concrete may be treated as a 5-component
 * evolving material.
 */
class HydratingConcreteMat : public IsotropicHeatTransferMaterial
{
public:
    HydratingConcreteMat(int n, Domain * d);
    virtual ~HydratingConcreteMat();

    virtual int hasInternalSource() override { return 1; }
    virtual int hasCastingTimeSupport() override { return 1; }
    virtual void computeInternalSourceVector(FloatArray &val, GaussPoint *gp, TimeStep *tStep, ValueModeType mode) override;

    double giveCharacteristicValue(MatResponseMode mode,
                                   GaussPoint *gp,
                                   TimeStep *tStep) override;

    const char *giveClassName() const override { return "HydratingConcreteMat"; }

    IRResultType initializeFrom(InputRecord *ir) override;

    // post-processing
    int giveIPValue(FloatArray &answer, GaussPoint *gp, InternalStateType type, TimeStep *tStep) override;
    double giveIsotropicConductivity(GaussPoint *gp, TimeStep *tStep) override;
    virtual double giveConcreteCapacity(GaussPoint *gp, TimeStep *tStep);
    virtual double giveConcreteDensity(GaussPoint *gp, TimeStep *tStep);
    /// Type of hydration model, e.g. exponential curve, Cervera's model.
    int hydrationModelType;
    double maxModelIntegrationTime;
    /// Minimum number of integration steps for hydration model within a given timeStep.
    double minModelTimeStepIntegrations;
    /// Potential heat of hydration, for ordinary Portland cement approximately 500 J/g.
    double Qpot;
    /// Mass of cement in kg per 1m3 of concrete.
    double massCement;
    /// Activation energy of concrete (default 38400 J/mol/K).
    double activationEnergy;
    /// Reference temperature for hydration model.
    double referenceTemperature;
    /**
     * Parameters for exponential affinity hydration model summarized in A.K. Schindler and K.J. Folliard:
     * Heat of Hydration Models for Cementitious Materials, ACI Materials Journal, 2005.
     */
    double tau, beta;

    /**
     * Parameters for affinity hydration model inspired by Cervera et al.
     * Journal of Engineering Mechanics ASCE, 125(9), 1018-1027, 1999.
     */
    double B1, B2, eta, DoHInf;
    ///Optional extension to slag-rich, high-blended cements
    double DoH1, P1;

protected:
    double GivePower(TimeStep *tStep, GaussPoint *gp, ValueModeType mode);
    double scaleTemperature(GaussPoint *gp);
    /// Return affinity scaled to 25C.
    double affinity25(double alpha);

    /// Use different methods to evaluate material conductivity, capacity, or density
    int conductivityType, capacityType, densityType;
    /// Degree of reinforcement, if defined, reinforcement effect for conductivity and capacity is accounted for. Isotropic case.
    double reinforcementDegree;
    MaterialStatus *CreateStatus(GaussPoint *gp) const override;
};

/**
 * HydratingConcreteMatStatus stores degree of hydration in each integration point
 */
class HydratingConcreteMatStatus : public TransportMaterialStatus
{
public:
    HydratingConcreteMatStatus(int n, Domain * d, GaussPoint * g);
    virtual ~HydratingConcreteMatStatus();
    /// Returns actual degree of hydration at last known equilibrium.
    double giveDoHActual();
    void updateYourself(TimeStep *tStep) override;
    void printOutputAt(FILE *file, TimeStep *tStep) override;
    double power;
    double lastEvalTime;
    double lastEquivalentTime, equivalentTime, degreeOfHydration, lastDegreeOfHydration;
};
} // end namespace oofem
#endif // hydratingconcretemat_h
