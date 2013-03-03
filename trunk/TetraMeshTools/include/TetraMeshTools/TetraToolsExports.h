/*
 * TetraToolsExports.h
 *
 *	Required for DLL build on Windows, won't do anything in Linux...
 *
 *  Created on: Jan 09, 2012
 *      Author: Dennis Luebke
 *
 * Set DLL exports
 *
 */
#ifndef __TETRATOOLSEXPORT_H_
#define __TETRATOOLSEXPORT_H_

#ifdef _WIN32
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT
#endif

#endif // __TETRATOOLSEXPORT_H_
