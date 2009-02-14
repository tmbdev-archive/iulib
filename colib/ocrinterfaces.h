#ifndef h_ocrinterfaces__
#define h_ocrinterfaces__

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
//
// You may not use this file except under the terms of the accompanying license.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You may
// obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Project: iulib -- image understanding library
// File: ocrinterfaces.h
// Purpose: interfaces to OCR system components
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file ocrinterfaces.h
/// \brief Interfaces to OCR system components


#include <stdio.h>
#include <stdlib.h>
#include "colib/narray.h"
#include "colib/narray-util.h"
#include "strbuf.h"
#include "smartptr.h"
#include "misc.h"
#include "coords.h"
#include "nustring.h"

namespace colib {

    /// Base class for OCR interfaces.

    /// Contains some minimal information
    /// and ways of interacting with an OCR component.

    struct IComponent {
        /// misc information logged about the history of the component
        strbuf object_history;

        /// brief description
        virtual const char *description() = 0;

        /// print longer info to stdout
        virtual void info(int depth=0,FILE *stream=stdout) {
            fprintf(stream,"%*s",depth,"");
            fprintf(stream,"%s\n",description());
            fprintf(stream,"%s\n",(const char *)object_history);
        }

        // virtual methods for getting and setting parameters

        /// Set a string property or throw an exception if not implemented.
        virtual void set(const char *key,const char *value) {
            throw "IComponent::set(char*,char*) unimplemented by subclass";
        }
        /// Set a number property or throw an exception if not implemented.
        virtual void set(const char *key,double value) {
            throw "IComponent::set(char*,double) unimplemented by subclass";
        }
        /// Get a string property or throw an exception if not implemented.
        virtual const char *gets(const char *key) {
            throw "IComponent::gets(char*) unimplemented by subclass";
        }

        /// Get a number property or throw an exception if not implemented.
        virtual double getd(const char *key) {
            throw "IComponent::getd(char*) unimplemented by subclass";
        }
        virtual ~IComponent() {}

    };

    /// Cleanup for gray scale document images.

    /// Should work for both gray scale and binary images.
    ///
    struct ICleanupGray : IComponent {
        /// Clean up a gray image.
        virtual void cleanup(bytearray &out,bytearray &in) = 0;
    };

    /// Cleanup for binary document images.

    /// Should throw an error when applied to grayscale.
    ///
    struct ICleanupBinary : IComponent {
        /// Clean up a binary image.
        virtual void cleanup(bytearray &out,bytearray &in) = 0;
    };

    /// Compute text/image probabilities

    /// The output is in the standard RGB format
    /// for text/image segmentation (see ocropus.org)

    struct ITextImageClassification : IComponent {
        /// Compute text/image probabilities.
        virtual void textImageProbabilities(intarray &out,bytearray &in) = 0;
    };

    /// Perform binarization of grayscale images.

    struct IBinarize : IComponent {
        /// Binarize an image stored in a floatarray. Override this.
        virtual void binarize(bytearray &out,floatarray &in) = 0;
        /// \brief Binarize an image stored in a bytearray.
        /// Override this if you want to provide a more efficient
        /// implementation.
        virtual void binarize(bytearray &out,bytearray &in) {
            floatarray temp;
            copy(temp,in);
            binarize(out,temp);
        }
    };

    /// Compute page segmentation into columns, lines, etc.

    /// The output is in the standard RGB format
    /// for page segmentation (see ocropus.org)

    struct ISegmentPage : IComponent {
        /// Segment the page.
        virtual void segment(intarray &out,bytearray &in) = 0;
        virtual void segment(intarray &out,bytearray &in,rectarray &obstacles)
            { throw "unimplemented"; }
    };

    /// Compute line segmentation into character hypotheses.
    //
    /// The output is in the standard RGB format
    /// for page segmentation (see ocropus.org)

    struct ISegmentLine : IComponent {
        /// Segment a line.
        virtual void charseg(intarray &out,bytearray &in) = 0;
    };

    /// \brief A generic interface for language models.

    /// An IGenericFst is a directed graph
    /// with output/cost/id written on arcs,
    /// accept cost written on vertices and
    /// a fixed start vertice.
    struct IGenericFst : virtual IComponent {
        /// Clear the language model
        virtual void clear() = 0;

        /// Get a single new state
        virtual int newState() = 0;

        /// Add a transition between the given states
        virtual void addTransition(int from,int to,int output,float cost,int input) = 0;

        /// A variant of addTransition() with equal input and output.
        virtual void addTransition(int from,int to,int symbol,float cost) {
            addTransition(from, to, symbol, cost, symbol);
        }

        /// Set the start state
        virtual void setStart(int node) = 0;

        /// Set a state as an accept state
        virtual void setAccept(int node,float cost=0.0) = 0;

        /// Obtain codes for "specials" (language model dependent)
        virtual int special(const char *s) = 0;

        /// \brief Compute the best path through the language model.
        /// Useful for simple OCR tasks and for debugging.
        virtual void bestpath(nustring &result) = 0;

        /// destroy the language model
        virtual ~IGenericFst() {}

        /// simple interface for line recognizers
        virtual void setString(nustring &text,floatarray &costs,intarray &ids) {
            int n = text.length();
            intarray states;
            states.clear();
            for(int i=0;i<n+1;i++)
                states.push(newState());
            for(int i=0;i<n;i++)
                addTransition(states[i],states[i+1],text[i].ord(),costs[i],ids[i]);
            setStart(states[0]);
            setAccept(states[n]);
        }

        // reading methods

        /// Get the number of states.
        virtual int nStates() { throw "unimplemented"; }

        /// Get the starting state.
        virtual int getStart() { throw "unimplemented"; }

        /// Get the accept cost of a given vertex (a cost to finish the line and quit).
        virtual float getAcceptCost(int node) { throw "unimplemented"; }

        /// Determine whether the given node is an accepting state.
        virtual bool isAccepting(int node) { return getAcceptCost(node)<1e30; }

        /// Return an array of arcs leading from the given node.
        virtual void arcs(colib::intarray &ids,
                          colib::intarray &targets,
                          colib::intarray &outputs,
                          colib::floatarray &costs,
                          int from) { throw "unimplemented"; } // WARN_DEPRECATED

        /// A variant of addTransition() with equal input and output.
        virtual void getTransitions(intarray &tos,intarray &symbols,floatarray &costs,intarray &inputs,int from) {
            arcs(inputs,tos,symbols,costs,from);
        }

        /// Change a transition score between the given states
        virtual void rescore(int from,int to,int output,float new_cost,int input) { throw "unimplemented"; }

        /// A variant of rescore() with equal input and output.
        virtual void rescore(int from, int to, int symbol, float new_cost) {
            rescore(from, to, symbol, new_cost, symbol);
        }

        /// These methods should load and save in OpenFST format.
        /// (A simple way of doing that is to convert internally to OpenFST,
        /// then call its load/save methods.)
        virtual void load(const char *file) = 0;
        virtual void save(const char *file) = 0;
    };

    /// A generic interface for isolated character recognizers.
    /// Note that this is not the preferred interface for character recognition,
    /// since feature extraction is quite inefficient if it's done a character at a time.

    struct ICharacterClassifier : IComponent {
        /// \brief Classify a character without any information about position on the line.
        ///
        /// May throw an exception if it's not implemented.
        virtual void setImage(bytearray &input_image) = 0;

        /// \brief Classify a character with information about position on the line.
        //
        /// May throw an exception if it's not implemented.
        virtual void setImage(bytearray &image,int base_y, int xheight_y, int descender_y, int ascender_y) = 0;

        /// Get the number of classes returned. Corresponds to indices to cls() and cost().
        virtual int length() = 0;

        /// Unicode character or character string.
        //
        /// Note that some classifiers may return multiple characters per class
        virtual void cls(nustring &result, int i) = 0;

        /// cost value for this classification; lower costs = better
        /// should aim to return negative log likelihoods
        virtual float cost(int i) = 0;

        /// "adaptation" means temporary adaptation of the classifier
        /// to all the characters between startTraining and finishTraining
        /// other types of training are recognizer-dependent
        virtual void startTraining(const char *type="adaptation") { throw "unimplemented"; }

        /// \brief Train a character.
        //
        /// (Commonly, this only stores data in the model; training is via an external program).
        /// This may be also train on ligatures (if supported),
        /// that's why `characters' is a nustring.
        virtual void addTrainingChar(bytearray &input_image,nustring &characters)
            { throw "unimplemented"; }

        /// Train a character.
        virtual void addTrainingChar(bytearray &image,int base_y, int xheight_y, int descender_y,
                int ascender_y,nustring &characters) { throw "unimplemented"; }

        /// Train a character in context (think about this some more).
        virtual void addTrainingChar(bytearray &image,bytearray &mask,nustring &characters)
                { throw "unimplemented"; }

        /// Finish training and switch back to recognition; this method may
        /// take a long time to complete.
        virtual void finishTraining() { throw "unimplemented"; }

        /// Save a trained model to the stream.
        virtual void save(FILE *stream) { throw "unimplemented"; }
        void save(const char *path) { save(stdio(path, "wb")); }

        /// Load a trained model from the stream.
        virtual void load(FILE *stream) { throw "unimplemented"; }
        void load(const char *path) { load(stdio(path, "rb")); }

        /// \brief Convenience function for getting the best output
        //
        /// (useful for debugging)
        virtual void best(nustring &result) {
            int mi = -1;
            float mc = 1e30;
            for(int i=0;i<length();i++) {
                if(cost(i)<mc) {
                    mi = i;
                    mc = cost(i);
                }
            }
            if(mi>=0)
                cls(result, mi);
            else
                result.clear();
        }

        /// destructor
        virtual ~ICharacterClassifier() {}
    };


    /// A generic interface for text line recognition.

    struct IRecognizeLine : IComponent {
        /// \brief Recognize a text line and return a lattice representing
        /// the recognition alternatives.
        virtual void recognizeLine(IGenericFst &result,bytearray &image) = 0;

        /// \brief Start training of the given type.

        /// "adaptation" means temporary adaptation of the classifier
        /// to all the lines between startTraining and finishTraining
        /// other types of training are recognizer-dependent
        virtual void startTraining(const char *type="adaptation") { throw "unimplemented"; }

        /// \brief Train on a text line.

        /// Usage is: call addTrainingLine with training data, then call finishTraining
        /// The state of the object is undefined between calling addTrainingLine and finishTraining, and it is
        /// an error to call recognizeLine before finishTraining completes.  This allows both batch
        /// and incemental training.
        /// NB: you might train on length 1 strings for single character training
        /// and might train on words if line alignment is not working
        /// (well, for some training data)
        virtual void addTrainingLine(bytearray &image,nustring &transcription) { throw "unimplemented"; }


        /// \brief Train on a text line, given a segmentation.
        /// This is analogous to addTrainingLine(bytearray,nustring) except that
        /// it takes the "ground truth" line segmentation.
        virtual void addTrainingLine(intarray &segmentation, bytearray &image_grayscale, nustring &transcription) { throw "unimplemented"; }


        /// Align a lattice with a transcription.
        /// \param[out] chars Characters along the best path.
        //                    Currently, every character in chars must have a corresponding
        //                    region in seg and the characters must be in reading order.
        //                    Eventually, chars may contain characters (e.g., spaces) that
        //                    do not correspond to any region.  Note that chars may not
        //                    correspond to any string allowed/suggested by the transcription.
        /// \param[out] seg Aligned segmentation, colors correspond to chars (starting at 1)
        /// \param[out] costs Costs corresponding to chars
        /// \param[in] image Input grayscale image
        /// \param[in] transcription The "ground truth" lattice to align
        virtual void align(nustring &chars,intarray &seg,floatarray &costs,
                           bytearray &image,IGenericFst &transcription) { throw "unimplemented"; }

        // eventually?
        // virtual void addTrainingLine(bytearray &image,IGenericFst &transcription) { throw "unimplemented"; }

        /// \brief Finish training, possibly making complex calculations.

        /// Call this when training is done and the system should switch back to recognition;
        /// this method may take a long time to complete.
        virtual void finishTraining() { throw "unimplemented"; }

        /// Save a trained model to the stream.
        virtual void save(FILE *stream) { throw "unimplemented"; }
        void save(const char *path) { save(stdio(path, "wb")); }

        /// Load a trained model from the stream.
        virtual void load(FILE *stream) { throw "unimplemented"; }
        void load(const char *path) { load(stdio(path, "rb")); }

        /// Destructor
        virtual ~IRecognizeLine() {}

        /// this is a weird, optional method that exposes character segmentation for those line recognizers that have it
        /// segmentation contains colored pixels, and a transition in
        /// the transducer of the form * --- 1/eps --> * --- 2/a --> *
        /// means that pixels with color 1 and 2 together form the
        /// letter "a"
        virtual void recognizeLine(intarray &segmentation,IGenericFst &result,bytearray &image)
            WARN_DEPRECATED { throw "unimplemented"; }

        // recognize a line with or without a given segmentation
        // if useit is set to true, the given segmentation is just displayed in loggers, but not used,
        // the segmenter computes the segmentation and the recognition uses its output
        // if useit is set to false, the segmenter is still launched for the loggers, but the given
        // segmentation is really used for the recognition
        virtual void recognizeLineSeg(intarray &segmentation,IGenericFst &result,
                                      bytearray &image, bool useit)
            WARN_DEPRECATED { throw "unimplemented"; }
    };
}

#endif
