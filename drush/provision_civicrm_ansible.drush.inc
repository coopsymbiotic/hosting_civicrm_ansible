<?php

/**
 * @file
 *   Aegir service autoloading function and switch based on version and include
 *   the appropriate files to handle install, verify, &c.
 */

# require_once('provision_civicrm_ansible.inc');
# require_once(dirname(__FILE__) . '/deploy.provision.inc');
# require_once(dirname(__FILE__) . '/install.provision.inc');
# require_once(dirname(__FILE__) . '/migrate.provision.inc');
require_once(dirname(__FILE__) . '/verify.provision.inc');

/**
 * Implements hook_drush_init().
 */
function provision_civicrm_ansible_drush_init() {
  // Register our service classes for autoloading.
  provision_civicrm_ansible_provision_register_autoload();
}

/**
 * Register our directory as a place to find Provision classes.
 *
 * This allows Provision to autoload our classes, so that we don't need to
 * specifically include the files before we use the class.
 */
function provision_civicrm_ansible_provision_register_autoload() {
  static $loaded = FALSE;
  if (!$loaded) {
    $loaded = TRUE;
    $list = drush_commandfile_list();
    $provision_dir = dirname($list['provision']);
    include_once($provision_dir . '/provision.inc');
    include_once($provision_dir . '/provision.service.inc');
    provision_autoload_register_prefix('Provision_', dirname(__FILE__));
  }
}

/**
 * Helper function to execute and log the output of an Ansible command
 *
 * @param $site String Ansible site
 * @param $tag String Ansible tag
 */
function provision_civicrm_ansible_execute($site, $tag) {
  $command = 'sudo /usr/local/bin/aegir-ansible ' . escapeshellarg($site) . ' ' . escapeshellarg($tag) . ' 2>&1';
  drush_log(dt('CiviCRM Ansible: running: %command', ['%command' => $command]), 'ok');

  // Using this rather complicated method so that ansible does not complain about non-blocking IO
  // Somewhat avoiding drush functions because they do not seem to work
  $descriptors = [
    // respectively stdin, stdout, stderr
    0 => ['pipe', 'r'],
    1 => ['pipe', 'w'],
    2 => ['pipe', 'w'],
  ];

  $process = proc_open($command, $descriptors, $pipes);

  if (is_resource($process)) {
    // Close input pipe
    fclose($pipes[0]);

    // Read output and error
    // ignoring error output because they were piped to stdout
    $output = stream_get_contents($pipes[1]);
    // $error = stream_get_contents($pipes[2]);

    // Close output and error pipes
    fclose($pipes[1]);
    fclose($pipes[2]);
    // Close the process
    $exitCode = proc_close($process);

    drush_log(dt('CiviCRM Ansible output...'), $exitCode == 0 ? 'ok' : 'error');
    $lines = explode("\n", $output);

    foreach ($lines as $line) {
      $line = trim($line);
      $line = preg_replace('/ \*+$/', '', $line);
      if (!$line) {
        continue;
      }
      if (preg_match('/^(fail|error|fatal)/i', $line)) {
        drush_log($line, 'error');
      }
      elseif (preg_match('/warning/i', $line)) {
        drush_log($line, 'warning');
      }
      else {
        drush_log($line, 'ok');
      }
    }
  }
}
