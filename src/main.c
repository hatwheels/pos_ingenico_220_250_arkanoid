#include "sdk30.h"
#include "inc/devices.h"
#include "inc/emvk.h"
#include "arkanoid.h"

// -------------------------------------------------------------------------------------------------------
// Prototype declaration to be used with INCENDO (replacing 'more_function' declared in SDK file 'etat.h').
// This new prototype can be used with SDK version >= 6.5.
// -------------------------------------------------------------------------------------------------------
// extern int more_function_ext(NO_SEGMENT no, S_ETATOUT *in, void *out);

/** Application has to call ServiceRegister for each service with a same
 *  address Main and using predefined service number.
 * \param    size (I-)
 *    - data size.
 * \param    data (I-)
 *    - contains data needed between the Manager and application regarding services.
 * \return
 *  service call status.
 * \header sdk30.h etat.h
 * \source main.c
 */

int Main (unsigned int size, StructPt * data)
{
  NO_SEGMENT      No;
  int             ret = FCT_OK;

/*
 * ...
 */

  arkanoid_menu ();

  return ret;
}
