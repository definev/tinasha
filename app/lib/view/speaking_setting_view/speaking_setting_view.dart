import 'dart:io';

import 'package:flutter/material.dart';
import 'package:tcp_client_dart/tcp_client_dart.dart';

class SpeakingSettingView extends StatefulWidget {
  const SpeakingSettingView({super.key});

  @override
  State<SpeakingSettingView> createState() => _SpeakingSettingViewState();
}

class _SpeakingSettingViewState extends State<SpeakingSettingView> {
  bool _isCasualMode = true; // Track the state of the switch
  Socket? socket;

  void connectServer(String message) async {
    var client =
        await TcpClient.connect('192.168.1.5', 3002, terminatorString: '');
    final data = await client.sendAndClose(message);
    print(data);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(''),
      ),
      body: Column(
        children: [
          ListTile(
            contentPadding: EdgeInsets.only(left: 20, right: 20, top: 10),
            title: Text('Change Mode'),
            subtitle: Text('Switch between the Casual Mode and the Focus Mode'),
            trailing: Switch(
              value: _isCasualMode,
              onChanged: (value) {
                setState(() {
                  _isCasualMode = value;
                  // Handle switch state change as needed

                  if (_isCasualMode) {
                    connectServer('mode:llm');
                  } else {
                    connectServer('mode:pdf');
                  }
                });
              },
            ),
          ),
          ListTile(
            title: Text('Adjust Volume'),
            subtitle: Text('Adjust the volume of the TinaVoice'),
            trailing: Icon(Icons.arrow_forward_ios),
          ),
          ListTile(
            title: Text('Select Tone'),
            subtitle: Text('Change the tone of the TinaVoice'),
            trailing: Icon(Icons.arrow_forward_ios),
          ),
          ListTile(
            title: Text('Change Prompt'),
            subtitle: Text('Change the prompt of the TinaVoice'),
            trailing: Icon(Icons.arrow_forward_ios),
          ),
          ListTile(
            title: Text('Change Mode'),
            subtitle: Text('Change the mode of the TinaVoice'),
            trailing: Icon(Icons.arrow_forward_ios),
          ),
        ],
      ),
    );
  }
}
