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
    
    var music:AVAudioPlayer = AVAudioPlayer()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        prepareAudios()
        music.play()
    }
    
    func prepareAudios() {
        
        var path = NSBundle.mainBundle().pathForResource("TrekSoundtrack", ofType: "mp3")
        music = AVAudioPlayer(contentsOfURL: NSURL(fileURLWithPath: path!), error: nil)
        music.prepareToPlay()
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }


}

