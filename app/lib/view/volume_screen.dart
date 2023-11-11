import 'package:flutter/material.dart';
import 'package:flutter_colorpicker/flutter_colorpicker.dart';

class VolumeScreen extends StatefulWidget {
  const VolumeScreen({Key? key}) : super(key: key);

  @override
  State<VolumeScreen> createState() => _VolumeScreenState();
}

class _VolumeScreenState extends State<VolumeScreen> {
  bool condition = true;
  String _deviceType = "Tinasha Smart Speaker";

  Color pickerColor = const Color(0xff443a49);
  Color currentColor = const Color(0xff443a49);

  double _slider = 0.0;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.white,
      appBar: AppBar(
        centerTitle: true,
        backgroundColor: Colors.white,
        foregroundColor: Colors.black,
        elevation: 0,
        title: Column(
          children: const [
            Text("Tinasha Smart Speaker"),
            // Text("Living room"),
          ],
        ),
        actions: [
          IconButton(
            onPressed: () {},
            icon: const Icon(
              Icons.more_horiz,
            ),
          )
        ],
      ),
      body: Column(
        children: [
          const SizedBox(
            height: 32,
          ),
          Container(
            height: 38,
            // width: 200,
            decoration: BoxDecoration(
                border: Border.all(
                  color: Colors.grey[300]!,
                ),
                borderRadius: BorderRadius.circular(24)),
            padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
            child: DropdownButton<String>(
              underline: Container(),
              items: const [
                DropdownMenuItem(
                  child: Text("Tinasha Smart Speaker"),
                  value: "Tinasha Smart Speaker",
                ),
                DropdownMenuItem(
                  child: Text("Tinasha Smart Speaker2"),
                  value: "Tinasha Smart Speaker2",
                ),
              ],
              onChanged: (value) {
                setState(() {
                  _deviceType = value ?? "";
                });
              },
              value: _deviceType,
            ),
          ),
          Container(
            margin: const EdgeInsets.all(16),
            padding: const EdgeInsets.symmetric(horizontal: 16, vertical: 4),
            decoration: BoxDecoration(
              color: Colors.blueGrey[50],
              borderRadius: BorderRadius.circular(8),
            ),
            child: Row(
              children: [
                const Text(
                  'Status',
                  style: TextStyle(
                    fontWeight: FontWeight.bold,
                  ),
                ),
                const Spacer(),
                Text(condition ? "On" : "Off"),
                Switch(
                    value: condition,
                    onChanged: (b) {
                      setState(() {
                        condition = b;
                      });
                    })
              ],
            ),
          ),
          ColorPicker(
            pickerColor: pickerColor,
            onColorChanged: (Color value) {
              setState(() {
                pickerColor = value;
              });
            },
            displayThumbColor: false,
            hexInputBar: false,
            enableAlpha: false,
            paletteType: PaletteType.hueWheel,
            portraitOnly: true,
          ),
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 16),
            child: Row(
              mainAxisAlignment: MainAxisAlignment.spaceBetween,
              children: [
                Icon(
                  Icons.volume_up,
                ),
                Text(
                  "${_slider.toStringAsFixed(0)}%",
                )
              ],
            ),
          ),
          Slider(
            value: _slider,
            min: 0.0,
            max: 100,
            onChanged: (d) {
              setState(() {
                _slider = d;
              });
            },
          ),
          // Text("Schedule time"),
        ],
      ),
    );
  }
}