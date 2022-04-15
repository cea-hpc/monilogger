import unittest
import monilog
import monilog._monilog as mnlg

class MyOtherEvent:
    pass

class MainTest(unittest.TestCase):

    def setUp(self):
        self.result = 0

    def test_undefined_event(self):
        @monilog.register("MyEvent")
        def monilogger(ctx):
            self.result = self.result + 1

        mnlg.define_basic_events({"MyEvent":0})
        mnlg.emit_event("MyEvent", mnlg.MoniLogExecutionContext())

        self.assertEqual(self.result, 1)

    def test_class_event(self):
        mnlg.define_basic_events({"MyOtherEvent":0})

        @monilog.register(MyOtherEvent)
        def monilogger(ctx):
            self.result = self.result + 1

        mnlg.emit_event("MyOtherEvent", mnlg.MoniLogExecutionContext())

        self.assertEqual(self.result, 1)

    def test_composite_event(self):
        mnlg.define_basic_events({"MyEvent":0, "MyOtherEvent":1})
        monilog.define_event("MyCompositeEvent", ["MyEvent", "MyOtherEvent"])

        @monilog.register("MyCompositeEvent")
        def monilogger(ctx):
            self.result = self.result + 1

        mnlg.emit_event("MyCompositeEvent", mnlg.MoniLogExecutionContext())
        
        self.assertEqual(self.result, 1)

if __name__ == '__main__':
    unittest.main()
