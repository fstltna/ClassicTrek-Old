//
//  ViewController.swift
//  Classic Trek
//
//  Created by Marisa Giancarla on 12/6/15.
//  Copyright © 2015 Pocketfiction. All rights reserved.
//

import UIKit
import AVFoundation

class ViewController: UIViewController {
    @IBOutlet var textView: UITextView!
    @IBAction func MoveButtonAction(sender: AnyObject) {
        
        var userinput = inputField.text
        print(userinput)
        // Do here whatever you want with the user's input!
    }
    
    @IBOutlet var inputField: UITextField!
    
    var bgMusic:AVAudioPlayer = AVAudioPlayer()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        var bgMusicURL:NSURL = NSBundle.mainBundle().URLForResource("TrekSoundtrack", withExtension: "mp3")!
        do { bgMusic = try AVAudioPlayer(contentsOfURL: bgMusicURL, fileTypeHint: nil) } catch _ { return print("file not found") }
        bgMusic.numberOfLoops = 1
        bgMusic.prepareToPlay()
        bgMusic.play()

        let inputfortextarea: String = ".  .  .  .  .  .  .  . \n.  .  .  .  .  .  .  . \n.  .  *  .  .  .  .  . \n.  .  .  .  *  .  .  . \n"
        // Whatever you pass in the updateTextView Function will be displayed in the textView
        updateTextView(inputfortextarea)
    }
    
    
    
    // Whatever passed in the updateTextView Function will be displayed in the textView
    func updateTextView(inputText: String) -> String {
        textView.text = inputText
        return inputText
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

