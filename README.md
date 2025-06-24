
## 📝 Sample Game Flow

### 👥 Login Phase

```
All players logging in...
Player 1 logged in.
Player 2 logged in.
Player 3 logged in.
Player 4 logged in.
```

---

### 🔄 Round 1

```
Question:
What is 2 + 2?

a) 4  
b) 3  
c) 5  
d) 22  

(Enter 0 to use a lifeline)
```

### 🧑 Player Answers

```
Player 1:
Choice: a

Player 2:
Choice: b

Player 3:
Choice: c

Player 4:
Choice: 0
```

---

### 🆘 Lifeline Activated (Player 4)

```
Lifeline Options:
1 - Ask a Friend  
2 - 50/50  
3 - Ask the Audience  

Player 4 Choice: 1

Friend: "I think it's a"
```

### 🔁 Reprompt After Lifeline

```
Please answer the question again:

a) 4  
b) 3  
c) 5  
d) 22  

Player 4:
Choice: d
```

---

### ✅ Result Announcement

```
Host: The correct answer is...  
Host: Letter d

Player 1 answered incorrectly.  
Player 2 answered incorrectly.  
Player 3 answered incorrectly.  
Player 4 answered correctly.
```

---

### 🧮 Score Computation

```
[Computing scores...]

1st - Player 4
2nd - Player 1
3rd - Player 2
4th - Player 3
```

---

## 🧩 Division of Work

| Part                    | Description                               | Assigned To |
| ----------------------- | ----------------------------------------- | ----------- |
| 1. Simulation & Threads | Game flow, thread logic, barrier, async   | Bea         |
| 2. Q\&A System & Timer  | Question display, scoring, chrono timer   | Denise      |
| 3. Login & Lifelines    | Latches for login, lifeline system        | —           |
| Overall Integration     | Use `mutex` to synchronize console output | Everyone    |

---

Let me know if you'd like a PDF export or `.md` file version.
