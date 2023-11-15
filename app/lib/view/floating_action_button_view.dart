import 'dart:convert';
import 'dart:math';
import 'package:app/view/chat/chat_view.dart';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:flutter/scheduler.dart';
import 'package:flutter_animate/flutter_animate.dart';
import 'package:open_app_file/open_app_file.dart';
import 'package:pdf_text/pdf_text.dart';
import 'package:http/http.dart' as http;

class FloatingActionButtonView extends StatefulWidget {
  const FloatingActionButtonView({super.key});

  @override
  State<FloatingActionButtonView> createState() =>
      _FloatingActionButtonViewState();
}

class _FloatingActionButtonViewState extends State<FloatingActionButtonView>
    with TickerProviderStateMixin {
  late final animationController = AnimationController(vsync: this);
  PDFDoc? _pdfDoc;
  String _text = "";
  PlatformFile? file;

  Future _pickPDFText() async {
    FilePickerResult? result = await FilePicker.platform.pickFiles();
    if (result != null) {
      _pdfDoc = await PDFDoc.fromPath(result.files.single.path!);
      setState(() {});
      await _readWholeDoc();
      await _postTextToEndpoint();
    }
  }

  Future _postTextToEndpoint() async {
    if (_text.isNotEmpty) {
      final response = await http.post(
        Uri.parse('https://tina.loca.lt/add_pdf/invoke'),
        headers: <String, String>{
          'Content-Type': 'application/json; charset=UTF-8',
          'Bypass-Tunnel-Reminder': 'thuhuh',
        },
        body: jsonEncode(<String, dynamic>{
          'input': {'content': _text},
        }),
      );

      if (response.statusCode == 200) {
        print('Text successfully posted to the endpoint');
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Text successfully posted to the endpoint'),
            duration: Duration(seconds: 3),
          ),
        );
      } else {
        print('Failed to post text to the endpoint');
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Failed to post text to the endpoint'),
            duration: Duration(seconds: 3),
          ),
        );
      }
    }
  }

  Future _readWholeDoc() async {
    if (_pdfDoc == null) {
      return;
    }

    String text = await _pdfDoc!.text;

    setState(() {
      _text = text;
    });
  }

  @override
  Widget build(BuildContext context) {
    final size = MediaQuery.of(context).size;
    return Material(
      color: Colors.transparent,
      child: Stack(
        children: [
          for (int i = 0; i < 3; i++)
            Positioned(
              bottom: 15,
              right: size.width / 2 - 32,
              child: ElevatedButton(
                style: ElevatedButton.styleFrom(
                  elevation: 0,
                  fixedSize: Size.square(56),
                  shape: CircleBorder(),
                ),
                onPressed: switch (i) {
                  0 => () {
                      Navigator.push(
                        context,
                        MaterialPageRoute(builder: (context) => ChatView()),
                      );
                    },
                  1 => () {},
                  2 => _pickPDFText,
                  _ => () {},
                },
                child: switch (i) {
                  0 => Icon(Icons.chat),
                  1 => Icon(Icons.mic),
                  2 => Icon(Icons.file_copy),
                  _ => null,
                },
              ),
            ).animate().move(
                  duration: 300.ms,
                  delay: 100.ms * i,
                  begin: Offset(0, 0),
                  end: switch (i) {
                    0 => Offset(-80, -20),
                    1 => Offset(0, -80),
                    2 => Offset(80, -20),
                    _ => Offset(0, 0),
                  },
                  curve: Curves.easeInBack,
                ),
          Positioned(
            bottom: 12,
            left: 0,
            right: 0,
            child: Center(
              child: FloatingActionButton(
                elevation: 0,
                child: Icon(Icons.add),
                onPressed: () async {
                  await animationController.reverse();
                  if (context.mounted) Navigator.pop(context);
                },
              )
                  .animate(controller: animationController)
                  .rotate(
                    duration: 300.ms,
                    begin: 0,
                    end: 0.125,
                    alignment: Alignment.center,
                    curve: Curves.easeIn,
                  )
                  .scaleXY(begin: 1.0, end: 1.2, curve: Curves.easeInBack),
            ),
          ),
        ],
      ),
    );
  }
}
