library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity ov7670_bram_capture is
    port (
        pclk  : in   std_logic;
        vsync : in   std_logic;
        href  : in   std_logic;
        d     : in   std_logic_vector (7 downto 0);
        addr  : out  std_logic_vector (17 downto 0);
        dout  : out  std_logic_vector (31 downto 0);
        we    : out  std_logic
    );
end ov7670_bram_capture;

architecture rtl of ov7670_bram_capture is
   signal d_latch      : std_logic_vector(31 downto 0) := (others => '0');
   signal address      : std_logic_vector(17 downto 0) := (others => '0');
   signal address_next : std_logic_vector(17 downto 0) := (others => '0');
   signal wr_hold      : std_logic_vector(3 downto 0)  := (others => '0');

begin
   addr <= address;
   process(pclk)
   begin
      if rising_edge(pclk) then
         if vsync = '1' then
            address <= (others => '0');
            address_next <= (others => '0');
            wr_hold <= (others => '0');
         else
            dout    <= d_latch;
            address <= address_next;
            we      <= wr_hold(3);
            wr_hold <= wr_hold(2 downto 0) & (href and not wr_hold(0) and not wr_hold(1) and not wr_hold(2));
            d_latch <= d_latch( 23 downto  0) & d;
            if wr_hold(3) = '1' then
               address_next <= address_next + 1;
            end if;
         end if;
      end if;
   end process;
end rtl;