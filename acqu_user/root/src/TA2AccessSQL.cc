//*************************************************************************
//* Author: Patrik Ott
//*************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TA2AccessSQL                                                         //
//                                                                      //
// Class used for reading the SQL config database.                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////





#include "TA2AccessSQL.h"

ClassImp(TA2AccessSQL)

Int_t	TA2AccessSQL::GetRunNumber()
{
	char*	str	= strpbrk(gAR->GetFileName(),".");
	while(strpbrk(str+1,"."))
		str	= strpbrk(str+1,".");
	
	*str	= '\0';
	str	= str-1;
	while(*str == '0' || *str == '1' || *str == '2' || *str == '3' || *str == '4' || *str == '5' || *str == '6' || *str == '7' || *str == '8' || *str == '9')
		str	= str-1;
	str	= str+1;
	
	fRunNumber	= atoi(str);
	printf("RunNumber : %d\n",fRunNumber);
	return fRunNumber;
}


//______________________________________________________________________________
TA2AccessSQL::TA2AccessSQL(const char* name, TA2Analysis* analysis)	: TA2Physics(name, analysis), fCaLibReader(0), fRunNumber(0), CBEnergyPerRunCorrection(false)
{
    // command-line recognition for SetConfig()
    AddCmdList(AccessSQLConfigKeys);
}



TA2AccessSQL::~TA2AccessSQL()
{
}

void TA2AccessSQL::SetConfig(Char_t* line, Int_t key)
{
    switch (key)
    {
    case ESQL_USE_CALIB:
        {   
            Char_t tmp[6][256];

            // read CaLib parameters
            if (sscanf(line, "%s%s%s%s%s", tmp[0], tmp[1], tmp[2], tmp[3], tmp[4]) == 5) 
            {
				//Get the run number
				//Int_t	len = strlen(gAR->GetFileName()) - 1;
				//while(len
				
				GetRunNumber();
				
                // create the CaLib reader
                fCaLibReader = new CaLibReader_t(tmp[0], tmp[1], tmp[2], tmp[3], tmp[4], fRunNumber);
                
                // check database connection
                if (fCaLibReader->IsConnected())
                    Info("SetConfig", "Using calibration '%s' in the CaLib database '%s' on '%s@%s'"
                                      , tmp[4], tmp[1], tmp[2], tmp[0]);
                else
                    Error("SetConfig", "Could not connect to CaLib database server!");
            }
            else Error("SetConfig", "CaLib could not be configured and is hence disabled!");
            break;
        }
    case ESQL_CALIB_MISC:
        {   
            Int_t target_pos;

            // read CaLib misc parameters
            if (sscanf(line, "%d", &target_pos) == 1)
            {
                // check if CaLib reader exists
                if (fCaLibReader) 
                {
                    fCaLibReader->SetTargetPosition((Bool_t) target_pos);
                }
                else
                {
                    Error("SetConfig", "Misc calibration cannot be configured because CaLib was not configured!");
                    break;
                }
            }
            else Error("SetConfig", "CaLib misc calibration could not be configured!");
            break;
        }
    case ESQL_CALIB_TAGG:
        {   
            Int_t time;

            // read CaLib TAGG parameters
            if (sscanf(line, "%d", &time) == 1)
            {
                // check if CaLib reader exists
                if (fCaLibReader) 
                {
                    fCaLibReader->SetTAGGtime((Bool_t) time);
                }
                else
                {
                    Error("SetConfig", "TAGG cannot be configured because CaLib was not configured!");
                    break;
                }
            }
            else Error("SetConfig", "CaLib TAGG calibration could not be configured!");
            break;
        }
    case ESQL_CALIB_CB:
        {   
            Int_t energy;
            Int_t time;
            Int_t timewalk;
            Int_t quadEnergy;

            // read CaLib CB parameters
            if (sscanf(line, "%d%d%d%d", &energy, &time, &timewalk, &quadEnergy) == 4)
            {
                // check if CaLib reader exists
                if (fCaLibReader)
                {
                    fCaLibReader->SetCBenergy((Bool_t) energy);
                    fCaLibReader->SetCBtime((Bool_t) time);
                    fCaLibReader->SetCBwalk((Bool_t) timewalk);
                    fCaLibReader->SetCBquadEnergy((Bool_t) quadEnergy);
                }
                else
                {
                    Error("SetConfig", "CB cannot be configured because CaLib was not configured!");
                    break;
                }
            }
            else Error("SetConfig", "CaLib CB calibration could not be configured!");
            break;
        }
    case ESQL_CALIB_TAPS:
        {   
            Int_t energy;  
            Int_t time;  
            Int_t quadEnergy;
            Int_t led;

            // read CaLib TAPS parameters
            if (sscanf(line, "%d%d%d%d", &energy, &time, &quadEnergy, &led) == 4)
            {
                // check if CaLib reader exists
                if (fCaLibReader)
                {
                    fCaLibReader->SetTAPSenergy((Bool_t) energy);
                    fCaLibReader->SetTAPStime((Bool_t) time);
                    fCaLibReader->SetTAPSquadEnergy((Bool_t) quadEnergy);
                    fCaLibReader->SetTAPSled((Bool_t) led);
                }
                else
                {
                    Error("SetConfig", "TAPS cannot be configured because CaLib was not configured!");
                    break;
                }
            }
            else Error("SetConfig", "CaLib TAPS calibration could not be configured!");
            break;
        }
    case ESQL_CALIB_PID:
        {   
            Int_t phi;
            Int_t droop;
            Int_t energy;
            Int_t time;

            // read CaLib PID parameters
            if (sscanf(line, "%d%d%d%d", &phi, &droop, &energy, &time) == 4)
            {
                // check if CaLib reader exists
                if (fCaLibReader)
                {
                    fCaLibReader->SetPIDphi((Bool_t) phi);
                    fCaLibReader->SetPIDdroop((Bool_t) droop);
                    fCaLibReader->SetPIDenergy((Bool_t) energy);
                    fCaLibReader->SetPIDtime((Bool_t) time);
                }
                else
                {
                    Error("SetConfig", "PID cannot be configured because CaLib was not configured!");
                    break;
                }
            }
            else Error("SetConfig", "CaLib PID calibration could not be configured!");
            break;
        }
    case ESQL_CALIB_VETO:
        {   
            Int_t energy;
            Int_t time;

            // read CaLib Veto parameters
            if (sscanf(line, "%d%d", &energy, &time) == 2)
            {
                // check if CaLib reader exists
                if (fCaLibReader)
                {
                    fCaLibReader->SetVetoenergy((Bool_t) energy);
                    fCaLibReader->SetVetotime((Bool_t) time);
                }
                else
                {
                    Error("SetConfig", "Veto cannot be configured because CaLib was not configured!");
                    break;
                }
            }
            else Error("SetConfig", "CaLib Veto calibration could not be configured!");
            break;
        }
    case ESQL_CALIB_CBENERGY_PER_RUN:
        {  
			Char_t tmp[128];
			
			if (sscanf(line, "%s", tmp) == 1) 
            {
				GetRunNumber();
				FILE*	f = fopen(tmp,"r");
				while(!feof(f))
				{
					Int_t 		num;
					Double_t 	val;
					if (fscanf(f, "%d %lf", &num, &val) == 1) 
					{
						if(num == fRunNumber)
						{
							CBEnergyPerRunCorrection		= true;
							CBEnergyPerRunCorrectionFactor	= val;
							break;
						}
					}
				}
				if(CBEnergyPerRunCorrection)
				{
					
					printf("CBEnergy correction factor for run %d is %lf\n", fRunNumber, CBEnergyPerRunCorrectionFactor);
				}
			}
		}
	default:
        {
            // default main apparatus SetConfig()
            TA2Physics::SetConfig(line, key);
            break;
        }
    }
}

void TA2AccessSQL::LoadDetectors(TA2DataManager* parent, Int_t depth)
{
    // Load recursively all detector components.
    
    Bool_t added = kFALSE;

    // get the children, leave if none found
    TList* children = parent->GetChildren();
    if (!children) return;
    
    // loop over children
    TObjLink *lnk = children->FirstLink();
    while (lnk)
    {
        TObject* obj = lnk->GetObject();
     
        // look for detectors
        if (!strcmp(obj->GetName(), "TAGG"))
        {
            fTagger = (TA2Tagger*) obj;
            added = kTRUE;
        }
        if (!strcmp(obj->GetName(), "FPD"))
        {
            fLadder = (TA2Ladder*) obj;
            added = kTRUE;
        }
        else if (!strcmp(obj->GetName(), "CB"))
        {
            fCB = (TA2CentralApparatus*) obj;
            added = kTRUE;
        }
        else if (!strcmp(obj->GetName(), "NaI"))
        {
            fNaI = (TA2CalArray*) obj;
            added = kTRUE;
        }
        else if (!strcmp(obj->GetName(), "PID"))
        {
            fPID = (TA2PlasticPID*) obj;
            added = kTRUE;
        }
        else if (!strcmp(obj->GetName(), "TAPS"))
        {
            fTAPS = (TA2Taps*) obj;
            added = kTRUE;
        }
        else if (!strcmp(obj->GetName(), "BaF2PWO"))
        {
            fBaF2PWO = (TA2TAPS_BaF2*) obj;
            added = kTRUE;
        }
        else if (!strcmp(obj->GetName(), "VETO"))
        {
            fVeto = (TA2TAPS_Veto*) obj;
            added = kTRUE;
        }

        // print information if a detector was added
        if (added)
        {
            for (Int_t i = 0; i < depth; i++) printf("  ");
            printf( "   %s (%s)\n", obj->GetName(), obj->ClassName());
            added = kFALSE;
        }

        // add children of child
        LoadDetectors((TA2DataManager*) obj, depth + 1);
        lnk = lnk->Next();
    }
}

void TA2AccessSQL::ApplyCaLib()
{
	if (!fCaLibReader->ApplyTargetPositioncalib(&fTargetPosition))
        Error("ApplyCaLib", "An error occured during target position calibration!");

    // calibrate TAGG
    if (fLadder)
    {
        if (!fCaLibReader->ApplyTAGGcalib(fLadder))
            Error("ApplyCaLib", "An error occured during tagger calibration!");
    }
    else Error("ApplyCaLib", "Tagger not found - could not calibrate the tagger using CaLib!");

    // calibrate CB
    if (fNaI)
    {
        if (!fCaLibReader->ApplyCBcalib(fNaI))
            Error("ApplyCaLib", "An error occured during CB calibration!");
    }
    else Error("ApplyCaLib", "CB not found - could not calibrate CB using CaLib!");

    // calibrate TAPS
    if (fBaF2PWO)
    {
        if (!fCaLibReader->ApplyTAPScalib(fBaF2PWO))
            Error("ApplyCaLib", "An error occured during TAPS calibration!");
    }
    else Error("ApplyCaLib", "TAPS not found - could not calibrate TAPS using CaLib!");
    
    // calibrate PID
    if (fPID)
    {
        if (!fCaLibReader->ApplyPIDcalib(fPID))
            Error("ApplyCaLib", "An error occured during PID calibration!");
    }
    else Error("ApplyCaLib", "PID not found - could not calibrate PID using CaLib!");
    
    // calibrate Veto 
    if (fVeto)
    {
        if (!fCaLibReader->ApplyVetocalib(fVeto))
            Error("ApplyCaLib", "An error occured during Veto calibration!");
    }
    else Error("ApplyCaLib", "Veto not found - could not calibrate Veto using CaLib!");
}

void TA2AccessSQL::PostInit()
{
	TA2Physics::PostInit();
	
	LoadDetectors(fParent, 0);
	
	if(fCaLibReader)
	{
		ApplyCaLib();
        	fCaLibReader->Deconnect();
	}
	
	if(CBEnergyPerRunCorrection)
	{
		for(int i=0; i<fNaI->GetNelement(); i++)
			fNaI->GetElement(i)->SetA1(CBEnergyPerRunCorrectionFactor * (fNaI->GetElement(i)->GetA1()));
	}
}

void TA2AccessSQL::LoadVariable()
{
	TA2Physics::LoadVariable();
}

void TA2AccessSQL::Reconstruct()
{
	TA2Physics::Reconstruct();
}