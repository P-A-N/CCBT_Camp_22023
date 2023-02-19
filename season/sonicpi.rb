use_real_time

##| サンプルの長さ
interval = 30

##| サンプルが入っているディレクトリ
##| samples = "/home/pi/import1"
samples = "/Users/horihiroyuki/import1"

##| list of current sample volumes
vols = [0,0,0,0,0,0,0,0,0,0,0,0]

##| max volume of samples
max_vols = [0.3,1,1,1,1,0.7,0.5,1,1,1,1,1]

control_names = [:jan_ctrl,
                 :feb_ctrl,
                 :mar_ctrl,
                 :apr_ctrl,
                 :may_ctrl,
                 :jun_ctrl,
                 :jul_ctrl,
                 :aug_ctrl,
                 :sep_ctrl,
                 :oct_ctrl,
                 :nov_ctrl,
                 :dec_ctrl]

##| サンプルを再生する
define :play_sample do |i|
  ##| 再生タイミングをずらすため１秒まつ
  sleep 1
  set control_names[i], (sample samples, i, amp: vols[i] * max_vols[i])
end


##| 音量を変更する
define :update_amp do |i, vol|
  if vol[i] != vols[i]
    control get[control_names[i]], amp: vol[i] * max_vols[i]
    vols[i] = vol[i]
  end
end


##| 全てのファイルを再生する
define :play_samples do
  play_sample(0)
  play_sample(1)
  play_sample(2)
  play_sample(3)
  play_sample(4)
  play_sample(5)
  play_sample(6)
  play_sample(7)
  play_sample(8)
  play_sample(9)
  play_sample(10)
  play_sample(11)
end

##| OSCを受信する
define :recv_osc do
  in_thread(name: :osc_recv) do
    loop do
      tmp = (sync "/osc*/vol")
      update_amp(0, tmp)
      update_amp(5, tmp)
      update_amp(1, tmp)
      update_amp(10, tmp)
      update_amp(3, tmp)
      update_amp(7, tmp)
      update_amp(9, tmp)
      update_amp(2, tmp)
      update_amp(6, tmp)
      update_amp(8, tmp)
      update_amp(4, tmp)
      update_amp(11, tmp)
    end
  end
  
end

##| ループ再生
live_loop :play do
  play_samples
  sleep interval - 11
end

recv_osc







