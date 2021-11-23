#pragma once

// The library of different types of UI that can be used as DashboardElement
// implementations are represented as string. 
//
// But it makes more sense, and is safer if there is a single point of authority
// (this file) and compiler guarded symbols (#defines) of what these values 
// actually are, so everything is agreeing on what these string values are.

#define DASHELENAME_DEFFLOAT	"deffloat"
#define DASHELENAME_DEFINT		"defint"
#define DASHELENAME_DEFENUM		"defenum"
#define DASHELENAME_DEFBOOL		"defbool"
#define DASHELENAME_DEFSTRING	"defstring"