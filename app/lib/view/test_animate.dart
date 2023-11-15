import 'package:flutter/material.dart';
import 'package:flutter_animate/flutter_animate.dart';

class TestAnimate extends StatelessWidget {
  const TestAnimate({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: SafeArea(
        child: Text("Hello")
            .animate()
            .fadeIn(duration: 600.ms)
            .then(delay: 200.ms) // baseline=800ms
            .slide(),
      ),
    );
  }
}
