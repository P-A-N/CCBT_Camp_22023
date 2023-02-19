
interval = 1


live_loop :playp do
use_real_time
interval = sync "/osc*/sensor"
end


live_loop :getter do
  sample :elec_ping
  sleep interval[0]
end
