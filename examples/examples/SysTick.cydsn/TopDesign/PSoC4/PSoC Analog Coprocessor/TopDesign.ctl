-- =============================================================================
-- The following directives assign pins to the locations specific for the
-- CY8CKIT-048 kit.
-- =============================================================================

-- === RGB LED ===
attribute port_location of LED_Min(0) : label is "PORT(1,4)"; -- RED LED
attribute port_location of LED_Sec(0) : label is "PORT(1,6)"; -- BLUE LED
 