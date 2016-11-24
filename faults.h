#ifndef _FAULTS_H_
#define _FAULTS_H_

#include "ch.h"
#include "datatypes.h"

void faults_init(void);
void faults_update(void);
void faults_set_fault(Fault fault);
void faults_clear_fault(Fault fault);
void faults_clear_all_faults(void);
uint8_t faults_get_faults(void);
bool faults_check_fault(Fault fault);
void faults_set_warning(Warning warning);
void faults_clear_warning(Warning warning);
void faults_clear_all_warnings(void);
uint8_t faults_get_warnings(void);
bool faults_check_warning(Warning warning);

#endif /* _FAULTS_H_ */
