

void setupCMU()
{
}

void cmu_enable_DAC()
{
  *CMU_HFPERCLKEN0 |= (1 << 17);
}

void cmu_disable_DAC()
{
  *CMU_HFPERCLKEN0 |= (1 << 17);
}
