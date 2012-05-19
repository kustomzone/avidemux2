// automatically generated by admSerialization.py, do not edit!
#include "ADM_default.h"
#include "ADM_paramList.h"
#include "ADM_coreJson.h"
#include "fluxsmooth.h"
bool  fluxsmooth_jserialize(const char *file, const fluxsmooth *key){
admJson json;
json.addUint32("temporal_threshold",key->temporal_threshold);
json.addUint32("spatial_threshold",key->spatial_threshold);
return json.dumpToFile(file);
};
bool  fluxsmooth_jdeserialize(const char *file, const ADM_paramList *tmpl,fluxsmooth *key){
admJsonToCouple json;
CONFcouple *c=json.readFromFile(file);
if(!c) {ADM_error("Cannot read json file");return false;}
bool r= ADM_paramLoadPartial(c,tmpl,key);
delete c;
return r;
};