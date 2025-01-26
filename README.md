# **Project Overview**

ParaLegal is a legal chatbot designed to provide users with relevant legal information based on their queries. This AI-powered tool is built to simplify access to legal knowledge by answering user questions in a conversational format.

The project leverages Gradio for the user interface, ensuring a seamless and intuitive experience for users who can easily interact with the chatbot in real time. Whether you're a student, professional, or anyone in need of quick legal guidance, ParaLegal offers a smart and easy way to get helpful information on a range of legal topics.

## Problem and Solution
### Problem:
Access to legal information can be challenging for many people. They are often complex and difficult to understand without proper legal knowledge. In addition, people may not always have immediate access to a lawyer or legal expert to answer their questions.

### Solution:
ParaLegal provides a simple and efficient way for users to get quick, relevant legal information. By interacting with the chatbot, users can input legal topics or questions, and receive clear and helpful explanations. The system was designed to provide information on common legal topics in a user-friendly format, empowering users to understand basic legal concepts without the need for a legal professional.

## Key features:
Instant, relevant legal advice based on user input.
A simple, interactive interface powered by Gradio.
Updates to improve responses and broaden the knowledge base.

ParaLegal aims to bridge the gap between legal information and accessibility, empowering individuals with the knowledge they need to understand and navigate legal topics confidently.

## **Technical Choices and Reasoning**

### **1. CrewAI Framework**
- **Why?** CrewAI provides a modular and scalable way to define agents and tasks. It allows us to break down complex problems into smaller, manageable tasks that can be handled by specialized agents.
- **Trade-offs**: While CrewAI is powerful, it requires careful configuration and tuning of agents and tasks. This can be time-consuming but pays off in terms of flexibility and scalability.

### **2. Gradio for UI**
- **Why?** Gradio is a lightweight and easy-to-use library for creating web-based interfaces. It allows us to quickly build a UI for interacting with the AI system without needing extensive frontend development.
- **Trade-offs**: Gradio is not as customizable as a full-fledged frontend framework like React or Vue.js. However, it is ideal for rapid prototyping and lightweight applications.

  ### **What Could Be Improved?**
- **Frontend**: If more time were available, a more sophisticated frontend (e.g., React or Vue.js) could be developed to provide a richer user experience.
- **Testing**: Additional unit and integration tests could be added to ensure the reliability of the system.
- **Deployment**: The project could be containerized using Docker for easier deployment and scaling.


## **Getting Started**

### **Prerequisites**
-  Python 3.10 or higher (must be `<3.13`)
- pip (Python package installer)
- crewai[tools]>=0.98.0,<1.0.0
Installation:

To run the project locally, follow these steps:


### **Installation**

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/para_legal_news.git
   cd para_legal_news/latest_ai_development/src/latest_ai_development

Make sure to export the `PYTHONPATH` to include the project directory:
  
  ```bash
  export PYTHONPATH=$(pwd)
  ```

Run the Gradio UI:
  ```bash
    python3 gradio_ui.py
  ```
## Acknowledgments
Thanks to the CrewAI team for the framework.
Special thanks to all contributors and users of this project. 
Members: madihanazar, nikhilshaji17, kambarg, nasqnik, mariame42
