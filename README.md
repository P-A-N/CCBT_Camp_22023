# CCBT_Camp_22023 team GAKKI

# Sonic Pi Introduction

<br/>

## How to play / stop  

<br/>
<img src = "imgs/run.jpg" width=500 border="1" /><br>
press run to play ( ⌘ + r )  
press stop to stop ( ⌘ + s )  

---

<br/>

## Make your synthesizer

<br/>

#### Your first synthesizer in Sonic Pi
```
play 60
```
<br/>

#### Change the type of sythesizer

```
use_synth :saw
play 60
sleep 1
use_synth :prophet
play 60
sleep 1
use_synth :tb303
play 60
sleep 1
use_synth :dsaw
play 60
sleep 1
use_synth :prophet
play 60
sleep 1
```
<br/>
Find your favorites from the list.

<img src = "imgs/synths.jpg" width=500 border="1" />

---
<br>

## Play sample
```
sample :ambi_lunar_land
```

### Try samples in Sonic Pi

Choose the type of samples from left and press the play buttons on the right.

<img src = "imgs/samples.jpg" width=500 border="1" />

---
<br>

## Create enverope
ADSR in Sonic Pi  
```
play 60, attack: 0.1, attack_level: 1, decay: 0.2, sustain_level: 0.4, sustain: 1, release: 0.5 
```
<img src = "imgs/adsr.jpg" width=700/>