#include "faults.h"

static uint8_t faults = FAULT_NONE;
static uint8_t warnings = WARNING_NONE;

void faults_set_fault(Fault fault)
{
    faults |= fault;
}

uint8_t faults_get_faults(void)
{
    return faults;
}

void faults_clear_fault(Fault fault)
{
    faults &= ~fault;
}

void faults_clear_all_faults(void)
{
    faults = FAULT_NONE;
}

bool faults_check_fault(Fault fault)
{
    return faults & fault;
}

void faults_set_warning(Warning warning)
{
    warnings |= warning;
}

uint8_t faults_get_warnings(void)
{
    return warnings;
}

void faults_clear_warnings(Warning warning)
{
    warnings &= ~warning;
}

void faults_clear_all_warnings(void)
{
    warnings = WARNING_NONE;
}

bool faults_check_warning(Warning warning)
{
    return warnings & warning;
}
