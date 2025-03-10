<?php
namespace IPP\Student\Exception;

use IPP\Core\Exception\IPPException;
use IPP\Core\ReturnCode;
use Throwable;

class InternalErrorException extends IPPException {
    public function __construct(?\Throwable $previous = null, bool $showTrace = true) {
        parent::__construct("Internal error in interpreter.php", ReturnCode::INTERNAL_ERROR, $previous, $showTrace);
    }
}