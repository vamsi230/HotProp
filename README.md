# 🕵️‍♂️ Prop Hunt (Hot Potato Twist)

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

## ▶️ How to Access the Build

This repository contains **source code only**.

👉 You can find the playable build and full portfolio here:
**[https://vamsidharmavarapu.vercel.app/](https://vamsidharmavarapu.vercel.app/)**

---

## 🌐 Multiplayer Setup (LAN)

This project is designed primarily for **LAN (Local Network) multiplayer testing**.

* Ensure all devices are on the **same network** (same Wi-Fi or router)
* One player hosts, others join using local session discovery

---

## 🧩 Troubleshooting (If Multiplayer Doesn’t Work)

If sessions are not found or connection fails, try the following:

* Disable **VPNs** (they often break local session discovery)
* In **View Network Connections (Windows)**:

  * Disable unused adapters (VPN adapters, virtual adapters, extra Ethernet/Wi-Fi)
* Make sure **Firewall** is not blocking the build (allow private network access)
* Ensure all players are on the **same subnet** (same router, not mobile hotspot + Wi-Fi mix)

These issues are common with Unreal Engine LAN setups.

---

## 🧪 Current Status

* ✅ Core gameplay implemented
* ✅ Multiplayer working
* 🔄 Polishing & feature expansion in progress

---

## 📌 Future Improvements

* More prop variety & maps
* UI/UX polish
* Sound design & feedback
* Matchmaking / lobby system
* Anti-cheese mechanics (exploit prevention)

---

## 🎯 What Makes It Interesting

Unlike traditional Prop Hunt:

* The hunter isn’t fixed
* Pressure rotates between players
* Creates **emergent gameplay + psychological tension**

---

## 📷 Preview

*(Add gameplay screenshots / GIFs here — highly recommended)*

---

## 👤 Author

**Vamsi (gamerthing4)**
Game Developer focused on gameplay systems & multiplayer mechanics

---

## 📄 License

MIT License
