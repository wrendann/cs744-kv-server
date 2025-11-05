# Project Quickstart Guide

This README provides the essential steps to get the project's server and database running and how to interact with the server using the client.

-----

## Prerequisites

Before you start, ensure you have the following installed on your system:

  * **Docker**
  * **Docker Compose**
  * **Python** (for running the client script)

-----

## Getting Started

### 1\. Launch the Server and Database

Use Docker Compose to build the necessary images and run the **PostgreSQL database** and the **server** in detached mode (in the background):

```bash
docker compose up --build -d
```

> **Note:** The first time you run this, it may take a few minutes as it downloads and builds the necessary images.

### 2\. Run the Client

Once the server is running, you can use the provided Python script to send requests to it.

Make sure you have **Python** installed. Then, execute the client script:

```bash
python client.py
```

-----

That's it\! You should now have the backend services running and a way to interact with them.

-----

Is there anything else you'd like to add or change in this README?