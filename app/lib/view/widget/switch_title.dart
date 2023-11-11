import 'package:flutter/material.dart';

class SwitchTile extends StatefulWidget {
  const SwitchTile({Key? key}) : super(key: key);

  @override
  State<SwitchTile> createState() => _SwitchTileState();
}

class _SwitchTileState extends State<SwitchTile> {
  bool _flag = false;

  @override
  Widget build(BuildContext context) {
    return SwitchListTile.adaptive(
      title: Text(_flag ? "On" : "Off"),
      value: _flag,
      onChanged: (b) {
        setState(() {
          _flag = b;
        });
      },
    );
  }
}
