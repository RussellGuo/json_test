/*
 * db9_init_for_factory.h
 *
 * setup DB9's pins with low to reduce the defeat rate.
 *
 *  Created on: Jan 27, 2021
 *      Author: Guo Qiang
 */

#ifndef __DB9_INIT_FOR_FACTORY_H__
#define __DB9_INIT_FOR_FACTORY_H__

#ifdef __cplusplus
extern "C" {
#endif

// On the production line, the DB9 test board may be plugged and unplugged under power,
// so try to keep its pins low to reduce the defect rate.
// But outside the production line, it is better to keep these pins open-drain input.
// a macro __FACTORY_RELEASE__ should be defined for factory release,
// otherwise the function do nothing
// parameter and value: none
void db9_init_for_factory(void);

#ifdef __cplusplus
}
#endif

#endif
