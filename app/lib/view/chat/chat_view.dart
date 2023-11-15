import 'package:flutter/material.dart';

class ChatView extends StatefulWidget {
  const ChatView({Key? key}) : super(key: key);

  @override
  _ChatViewState createState() => _ChatViewState();
}

class _ChatViewState extends State<ChatView> {
  final TextEditingController _messageController = TextEditingController();
  final List<ChatMessage> _messages = [
    ChatMessage(text: 'Hello! How can I help you?', isBot: true),
    ChatMessage(text: 'This is a sample response from the bot.', isBot: true),
  ];

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Chat with Bot'),
      ),
      body: Column(
        children: [
          Expanded(
            child: ListView.builder(
              itemCount: _messages.length,
              itemBuilder: (context, index) {
                return MessageBubble(message: _messages[index]);
              },
            ),
          ),
          _buildInputField(),
        ],
      ),
    );
  }

  Widget _buildInputField() {
    return Padding(
      padding: const EdgeInsets.only(left: 12.0, right: 12.0, bottom: 40.0),
      child: Container(
        decoration: BoxDecoration(
          border: Border.all(color: Colors.grey),
          borderRadius: BorderRadius.circular(20.0),
        ),
        child: Row(
          children: [
            Expanded(
              child: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 12.0),
                child: TextField(
                  controller: _messageController,
                  decoration: InputDecoration(
                    hintText: 'Type a message...',
                    border: InputBorder.none,
                  ),
                ),
              ),
            ),
            IconButton(
              icon: Icon(Icons.send),
              onPressed: () {
                _sendMessage(_messageController.text);
              },
            ),
          ],
        ),
      ),
    );
  }

  void _sendMessage(String message) {
    setState(() {
      _messages.add(ChatMessage(text: message, isBot: false));
      // Simulate a bot response for demonstration purposes
      _messages.add(ChatMessage(text: 'Thanks for your message!', isBot: true));
      _messageController.clear();
    });
  }
}

class MessageBubble extends StatelessWidget {
  final ChatMessage message;

  const MessageBubble({required this.message});

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(8.0),
      child: Align(
        alignment: message.isBot ? Alignment.centerLeft : Alignment.centerRight,
        child: Container(
          decoration: BoxDecoration(
            color: message.isBot ? Colors.blue : Colors.green,
            borderRadius: BorderRadius.circular(8.0),
          ),
          padding: const EdgeInsets.all(12.0),
          child: Text(
            message.text,
            style: TextStyle(color: Colors.white),
          ),
        ),
      ),
    );
  }
}

class ChatMessage {
  final String text;
  final bool isBot;

  ChatMessage({required this.text, required this.isBot});
}
