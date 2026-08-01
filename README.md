# 🕵️‍♂️ Prop Hunt (Hot Potato Twist)

⚠️ **This repository contains source code only. Full playable build is available on my portfolio.**

A multiplayer party game inspired by classic Prop Hunt — but with a **Hot Potato mechanic** that constantly shifts pressure between players.

---

## 🎮 Core Idea

Players disguise themselves as props and hide from the hunter.

> **You don’t want to be the hunter when the timer ends.**

The “hunter” role is dynamically transferable, turning the game into a **high-tension mind game** instead of a simple hide-and-seek.

---

## 🚀 Features

* 🧍‍♂️ **Multiplayer Gameplay**
  * Real-time player interactions
  * Role switching (Prop ↔ Hunter)

* 🎭 **Prop Disguise System**
  * Transform into environmental objects
  * Blend into surroundings strategically

* 🔥 **Hot Potato Mechanic**
  * Hunter role transfers between players
  * Constant pressure and dynamic gameplay

* 🧠 **Strategic Depth**
  * Not just hiding — timing and positioning matter
  * Players must decide when to stay hidden or take risks

---

## 🛠️ Tech Stack

* **Engine:** Unreal Engine 5  
* **Language:** C++ / Blueprints  
* **Networking:** UE Multiplayer Framework (Replication, RPCs)

---

## ⚙️ Key Systems Implemented

* 🔄 **Dynamic Role Transfer System**
  * Hunter role reassignment (Hot Potato logic)
  * Timer-driven win condition tied to role state

* 🌐 **Multiplayer Networking (UE5)**
  * Replication of player states and transforms
  * RPCs for role switching, interactions, and events
  * Session hosting & discovery (LAN)

* 🎭 **Prop Morph System**
  * Runtime mesh switching with collision adjustments
  * Alignment & rotation to match environment

* 🎯 **Interaction & Detection System**
  * Hunter interaction checks (hit/trace-based)
  * Feedback for correct/incorrect detection

* 🎮 **Player Controller & Camera Handling**
  * Seamless control between prop and hunter states
  * Camera adaptation based on role

* 📊 **Game State Management**
  * Centralized match flow (start, timer, end conditions)
  * Player role tracking and synchronization

---

## 🧠 Challenges Solved

* Handling **role desync issues** during multiplayer role transfer  
* Ensuring **reliable state replication** across all clients  
* Managing **prop transformation** with correct collision + alignment  
* Preventing edge cases during **mid-timer role switching**  
* Maintaining consistent gameplay under **network latency conditions**

---

## 📍 Where to Start (Code Overview)

* `PH_GameMode` → Core match flow & rules  
* `PH_Player_Prop` → Prop behavior & transformation  
* `PH_MorphComponent` → Morph system logic  
* `PH_PlayerController` → Input & role handling  

---

## ▶️ How to Access the Build

This repository contains **source code only**.

👉 Playable build & full portfolio:  
**https://vamsidharmavarapu.vercel.app/**

---

## 🌐 Multiplayer Setup (LAN)

This project is designed primarily for **LAN (Local Network) multiplayer testing**.

* Ensure all devices are on the **same network**  
* One player hosts, others join via session discovery  

---

## 🧩 Troubleshooting (If Multiplayer Doesn’t Work)

* Disable **VPNs**  
* Disable unused adapters in **Network Connections (Windows)**  
* Allow the app through **Firewall (Private Network)**  
* Ensure all devices are on the **same subnet**  

---

## 🧪 Current Status

* ✅ Core gameplay implemented  
* ✅ Multiplayer working  
* 🔄 Polishing & feature expansion in progress  

---


## 🎯 What Makes It Interesting

Unlike traditional Prop Hunt:

* The hunter isn’t fixed  
* Pressure rotates between players  
* Creates **emergent gameplay + psychological tension**

---

## 👤 Author

**Vamsi (gamerthing4)**  
Game Developer focused on gameplay systems & multiplayer mechanics  

---

## 📄 License

MIT License  
